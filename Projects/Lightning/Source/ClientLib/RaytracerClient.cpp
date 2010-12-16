#include <RaytracerClient.h>
#include <Pxf/Base/Platform.h>

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

				raytracer::Task *task_data = new raytracer::Task();
				task_data->ParseFromString((req->task)->task());

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
				for(size_t i = 0; i < 256; ++i)
					blob.samples[i] = blob_proto->samples(i);
				blob.bounce_count = blob_proto->bounce_count();

				blob.pic_w = blob_proto->pic_w();
				blob.pic_h = blob_proto->pic_h();

				blob.samples_per_pixel = blob_proto->samples_per_pixel();

				blob.interleaved_feedback = blob_proto->interleaved_feedback();

				for(size_t i = 0; i < blob.prim_count; i++)
				{
					// TODO: lol, jhonnys grejer.
				}

				int sub_tasks_left = blob.interleaved_feedback*blob.interleaved_feedback;
				render_result_t out;

				while (sub_tasks_left > 0)
				{
					render_task(&task, &blob, &out, blob.interleaved_feedback*blob.interleaved_feedback - sub_tasks_left);

					TaskResult* res = new TaskResult();
					res->task = req;
					raytracer::Result *ray_res = new raytracer::Result();
					ray_res->set_id(task_data->id());
					ray_res->set_final((sub_tasks_left == 1));
					ray_res->set_data(Util::String((char*)out.data, sizeof(pixel_data_t)*task_data->h()*task_data->w()*3));

					client::Result *res_proto = new client::Result();
					res_proto->set_batchhash(req->batch->hash);
					res_proto->set_result(ray_res->SerializeAsString());

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
	m_NumWorkers = Platform::GetNumberOfProcessors();
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
