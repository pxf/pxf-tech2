#include <RaytracerClient.h>
#include <Pxf/Base/Platform.h>

#include <Renderer.h>

#ifdef CONF_FAMILY_WINDOWS
	#include <windows.h>
#endif

using namespace ZThread;
using namespace Pxf;

class Worker : public Runnable
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
				TaskRequest* req = m_Client->pop_request();

				if (!req)
				{
					m_Canceled = true;
					break;
				}

				
				batch_blob_t* blob  = req->blob;
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
				}

				
			}
			catch (Cancellation_Exception)
			{
				break;
			}
			catch (Interrupted_Exception* e)
			{
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
	m_Executor = new PoolExecutor(m_NumWorkers);
	m_TaskQueue.register_type(LightningClient::RayTraceTask);
}

RaytracerClient::~RaytracerClient()
{
	m_Executor->interrupt();
	m_Executor->wait();
	delete m_Executor;
}

void RaytracerClient::push_request(TaskRequest* _Request)
{
	m_TaskQueue.push(LightningClient::RayTraceTask, _Request);
}

TaskRequest* RaytracerClient::pop_request()
{
	return m_TaskQueue.pop(LightningClient::RayTraceTask);
};

void RaytracerClient::push_result(TaskResult* _Result)
{
	m_ResultQueue.add(_Result);
}

TaskResult* RaytracerClient::pop_result()
{
	return m_ResultQueue.next();
}

bool RaytracerClient::has_results()
{
	return !m_ResultQueue.empty();
}

bool RaytracerClient::run()
{
	run_noblock();

	try
	{

		m_Executor->wait();
		m_Kernel->Log(m_LogTag, "Executor is done waiting. Why?");
	}
	catch(Cancellation_Exception* e)
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
	m_TaskQueue.cancel();
	m_Executor->cancel();
}

void RaytracerClient::interrupt()
{
	m_Executor->interrupt();
}