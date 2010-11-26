#include <RaytracerClient.h>
#include <Pxf/Base/Platform.h>
#include <ZThread/PoolExecutor.h>

#include <windows.h>

using namespace ZThread;
using namespace Pxf;

/*
	auxthread: Reciever, global inQueue
	auxthread: Transmitter, global outQueue
	thread{numcpu}: Worker, get from inQueue, process, put in outQueue 
*/

class Reciever : public Runnable
{
public:
	void run()
	{
		// wait for incomming jobs
		// put job in inQueue
		// repeat
	}
};

class Transmitter : public Runnable
{
public:
	void run()
	{
		// wait for completed jobs in outQueue
		// send job to whereever
		// repeat	
	}
};

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
			JobRequest* req = m_Client->GetRequest();

			JobResult* res = 0;
			m_Client->PutResult(res);

			
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

bool RaytracerClient::Run()
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