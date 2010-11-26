#include <RaytracerClient.h>
#include <Pxf/Base/Platform.h>
#include <ZThread/PoolExecutor.h>

#include <windows.h>

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
		while(true)
		{
			JobRequest* req = m_Client->get_request();

			JobResult* res = 0;
			m_Client->put_result(res);

			
		}
	}
};


RaytracerClient::RaytracerClient(Pxf::Kernel* _Kernel)
	: m_Kernel(_Kernel)
{

}

RaytracerClient::~RaytracerClient()
{

}

bool RaytracerClient::run()
{
	unsigned tag = m_Kernel->CreateTag("RTC");
	int num_workers = Platform::GetNumberOfProcessors();
	PoolExecutor executor(num_workers);

	for(int i = 0; i < num_workers; i++)
		executor.execute(new Worker(this));

	try
	{
		while(true)
		{
			executor.wait();
			m_Kernel->Log(tag, "Executor is done waiting. Why?");
		}
	}
	catch(Cancellation_Exception* e)
	{
		return false;
	}

	return true;
}