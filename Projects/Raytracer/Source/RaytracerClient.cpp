#include <RaytracerClient.h>
#include <Pxf/Base/Platform.h>

#ifdef CONF_FAMILY_WINDOWS
	#include <windows.h>
#endif

using namespace ZThread;
using namespace Pxf;

class Worker : public Runnable
{
protected:
	LightningClient* m_Client;
public:
	Worker(LightningClient* _Client)
		: m_Client(_Client)
	{}

	void run()
	{
		while (true)
		{
			try
			{
				TaskRequest* req = m_Client->pop_request();

				TaskResult* res = 0;
				m_Client->push_result(res);
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

bool RaytracerClient::run()
{
	run_noblock();

	for(int i = 0; i < 10; i++)
	{
		m_TaskQueue.push(LightningClient::RayTraceTask, new TaskRequest());
	}

	try
	{
		while(true)
		{
			m_Executor->wait();
			m_Kernel->Log(m_LogTag, "Executor is done waiting. Why?");
		}
	}
	catch(Cancellation_Exception* e)
	{
		return false;
	}

}

bool RaytracerClient::run_noblock()
{
	for(int i = 0; i < m_NumWorkers; i++)
		m_Executor->execute(new Worker(this));

	return true;
}