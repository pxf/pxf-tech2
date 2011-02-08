#include <RaytracerClient.h>
#include <Pxf/Base/Platform.h>
#include <string>

#include <Renderer.h>

#ifdef CONF_FAMILY_WINDOWS
	#include <windows.h>
#endif

using namespace Pxf;

class Worker : public ZThread::Runnable
{
protected:
	LightningClient* m_Client;
	Pxf::Kernel* m_Kernel;
	bool m_Canceled;
public:
	Worker(Pxf::Kernel* _Kernel, LightningClient* _Client)
		: m_Kernel(_Kernel)
		, m_Client(_Client)
		, m_Canceled(false)
	{}

	void run()
	{
		while (!m_Canceled)
		{
			try
			{
				Task* req = m_Client->pop_request();
				//req->task->PrintDebugString();

				raytracer::Task *task_data = new raytracer::Task();
				//printf("raw task data: %s\n", (req->task)->DebugString());
				//(req->task)->PrintDebugString();
				
				if (!task_data->ParseFromString((req->task)->task()))//Array((const void*)(req->task)->task().c_str(), (req->task)->tasksize()))
				{
					printf("FAILED PARSE!\n");
				}
				task_data->PrintDebugString();
				//printf("size lols: %d\n", task_data->ByteSize());

				//printf("h: %d   w: %d\n", task_data->h(), task_data->w());
				task_detail_t task;
				task.region[0] = task_data->x();
				task.region[1] = task_data->y();
				task.region[2] = task_data->x() + task_data->h();
				task.region[3] = task_data->y() + task_data->w();
				raytracer::DataBlob *blob_proto = new raytracer::DataBlob();
				blob_proto->ParseFromString(req->batch->data);

				// Create the blob from the DataBlob-proto.
				batch_blob_t blob;
				blob.prim_count = blob_proto->prim_count();
				blob.light_count = blob_proto->light_count();
				//printf("count: %d\n", blob.light_count);
				for(size_t i = 0; i < 256; ++i)
					blob.samples[i] = blob_proto->samples(i);
				blob.bounce_count = blob_proto->bounce_count();

				blob.pic_w = blob_proto->pic_w();
				blob.pic_h = blob_proto->pic_h();

				blob.samples_per_pixel = blob_proto->samples_per_pixel();

				blob.interleaved_feedback = blob_proto->interleaved_feedback();
				
				// unpack camera pos and orientation
				SimpleCamera cam;
				Pxf::Math::Quaternion camorient;
				camorient.x = blob_proto->cam().orient_x();
				camorient.y = blob_proto->cam().orient_y();
				camorient.z = blob_proto->cam().orient_z();
				camorient.w = blob_proto->cam().orient_w();
				cam.SetOrientation(&camorient);
				cam.SetPosition(blob_proto->cam().position().x(), blob_proto->cam().position().y(), blob_proto->cam().position().z());
				blob.cam = &cam;
				
				// unpack primitives
				std::string mData = blob_proto->materials();
				std::string pData = blob_proto->primitive_data();

				triangle_t* triangles = (triangle_t*) pData.c_str();
				MaterialLibrary* materials = (MaterialLibrary*) mData.c_str();

				blob.primitives = triangles;
				blob.materials = *materials;

				/*
				for(size_t i = 0; i < blob.prim_count; i++)
				{
					material_t *sphere_mat1 = new material_t();

					sphere_mat1->ambient = Math::Vec3f(0.1f, 0.1f, 0.1f);
					sphere_mat1->diffuse = Math::Vec3f(1.0f, 1.0f, 1.0f);
					sphere_mat1->reflectiveness = 1.0f;
					
					// TODO: lol, jhonnys grejer.
					//raytracer::DataBlob::PrimitiveSphere *pb_sphere = blob_proto->primitives(i);
					raytracer::DataBlob::Vec3f pb_pos = blob_proto->primitives(i).position();
					blob.primitives[i] = Sphere(Pxf::Math::Vec3f(pb_pos.x(), pb_pos.y(), pb_pos.z()), blob_proto->primitives(i).size(), 0);
				}*/
				
				// unpack lights!
				for(size_t i = 0; i < blob.light_count; i++)
				{
					/*
					material_t *light_mat1 = new material_t();

					light_mat1->ambient = Math::Vec3f(0.1f, 0.1f, 0.1f);
					light_mat1->diffuse = Math::Vec3f(1.0f, 1.0f, 1.0f);

					*/
					
					// TODO: lol, jhonnys grejer.
					raytracer::DataBlob::Vec3f pb_pos = blob_proto->lights(i).position();
					blob.lights[i] = new PointLight(Pxf::Math::Vec3f(pb_pos.x(), pb_pos.y(), pb_pos.z()), 0);
				

				//printf("if %d\n", blob.interleaved_feedback);
				}

				int sub_tasks_left = blob.interleaved_feedback*blob.interleaved_feedback;
				//sub_tasks_left = 1;
				render_result_t out;

				printf("started rendering task id: %d (sub_task_left: %d)\n", task_data->id(), sub_tasks_left);
				while (sub_tasks_left > 0)
				{
					render_task(&task, &blob, &out, blob.interleaved_feedback*blob.interleaved_feedback - sub_tasks_left);
					printf("done rendering task id: %d (sub_task_left: %d)\n", task_data->id(), sub_tasks_left);

					TaskResult* res = new TaskResult();
					
					res->task = req;
					
					raytracer::Result *ray_res = new raytracer::Result();
					ray_res->set_id(task_data->id());
					ray_res->set_final((sub_tasks_left == 1));
					ray_res->set_x(task_data->x());
					ray_res->set_y(task_data->y());
					ray_res->set_w(task_data->w());
					ray_res->set_h(task_data->h());
					ray_res->set_size(sizeof(pixel_data_t)*task_data->h()*task_data->w());
					ray_res->set_data(Util::String((char*)out.data, sizeof(pixel_data_t)*task_data->h()*task_data->w()));

					client::Result *res_proto = new client::Result();
					res_proto->set_batchhash(req->batch->hash);
					res_proto->set_result(ray_res->SerializeAsString());
					
					res->result = res_proto;

					printf("pushing result on queue (%d).\n", sub_tasks_left);
					m_Client->push_result(res);

					sub_tasks_left--;
				}
				


/*				batch_blob_t* blob  = req->blob;
				task_detail_t task;
				task.region[0] = req->rect.x;
				task.region[1] = req->rect.y;
				task.region[2] = req->rect.x + req->rect.h;
				task.region[3] = req->rect.y + req->rect.w;
				render_result_t out;
				int sub_tasks_left = blob->interleaved_feedback*blob->interleaved_feedback;
				
				while (sub_tasks_left > 0)
				{
					sub_tasks_left = render_task(&task, blob, &out, blob->interleaved_feedback*blob->interleaved_feedback - sub_tasks_left);
					
					TaskResult* res = new TaskResult;
					res->rect = req->rect;
					res->pixels = (uint8*)out.data;
					if (sub_tasks_left == 0)
						res->final = true;
					else
						res->final = false;
					m_Client->push_result(res);
				}*/

				
			}
			catch (ZThread::Cancellation_Exception)
			{
				m_Canceled = true;
				break;
			}
		}
	}
};


RaytracerClient::RaytracerClient(Pxf::Kernel* _Kernel)
	: m_Kernel(_Kernel)
	, m_Executor(0)
	, m_LogTag(0)
	, m_NumWorkers(1)
{
	m_LogTag = m_Kernel->CreateTag("RTC");
	m_NumWorkers = 1;//Platform::GetNumberOfProcessors();
	m_Executor = new ZThread::PoolExecutor(m_NumWorkers);
}

void RaytracerClient::initialize(BlockingTaskQueue<Task*>* _TaskQueue, ZThread::BlockingQueue<TaskResult*, ZThread::FastMutex>* _ResultQueue)
{
	m_TaskQueue = _TaskQueue;
	m_ResultQueue = _ResultQueue;
	m_TaskQueue->register_type(LightningClient::RayTraceTask);
}

RaytracerClient::~RaytracerClient()
{
	cancel();
	m_Executor->wait();
	delete m_Executor;
}

void RaytracerClient::push_request(Task* _Request)
{
	m_TaskQueue->push(LightningClient::RayTraceTask, _Request);
}

Task* RaytracerClient::pop_request()
{
	return m_TaskQueue->pop(LightningClient::RayTraceTask);
};

void RaytracerClient::push_result(TaskResult* _Result)
{
	m_ResultQueue->add(_Result);
}

TaskResult* RaytracerClient::pop_result()
{
	return m_ResultQueue->next();
}

bool RaytracerClient::has_results()
{
	return !m_ResultQueue->empty();
}

bool RaytracerClient::run()
{
	run_noblock();

	try
	{
		m_Executor->wait();
		m_Kernel->Log(m_LogTag, "Executor is done waiting. Why?");
	}
	catch(ZThread::Cancellation_Exception* e)
	{
		m_Kernel->Log(m_LogTag, "Executor cancelled.");
		return false;
	}
	return false;
}

bool RaytracerClient::run_noblock()
{
	for(int i = 0; i < m_NumWorkers; i++)
		m_Executor->execute(new Worker(m_Kernel, this));

	return true;
}

bool RaytracerClient::wait()
{
	m_Executor->wait();
	return true;
}

void RaytracerClient::cancel()
{
	m_TaskQueue->cancel();
	m_Executor->cancel();
}
