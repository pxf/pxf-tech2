#ifndef _RAYTRACER_CLIENT_H_
#define _RAYTRACER_CLIENT_H_

#include <Pxf/Base/Types.h>
#include <Pxf/Kernel.h>

// todo: forward-declare these and use pointers to minimize header dependencies.
#include <zthread/PoolExecutor.h>
#include <zthread/BlockingQueue.h>
#include <zthread/FastMutex.h>

class Rect
{
public:
	int x, y;
	int w, h;

	Rect()
		: x(0), y(0)
		, w(0), h(0)
	{}
};


class JobRequest
{
public:
	Rect rect;
};

class JobResult
{
	uint8* pixels;
};

typedef ZThread::BlockingQueue<JobRequest*, ZThread::FastMutex> JobRequestQueue;
typedef ZThread::BlockingQueue<JobResult*, ZThread::FastMutex> JobResultQueue;

class LightningClient
{
protected:
	JobRequestQueue m_queue_in;
	JobResultQueue m_queue_out;
public:
	JobRequest* get_request()
	{
		return m_queue_in.next();
	}

	void put_result(JobResult* _Result)
	{
		m_queue_out.add(_Result);
	}
};

class RaytracerClient : public LightningClient
{
private:
	Pxf::Kernel* m_Kernel;
	ZThread::PoolExecutor* m_Executor;
	unsigned m_NumWorkers;
	unsigned m_LogTag;
public:
	RaytracerClient(Pxf::Kernel* _Kernel);
	~RaytracerClient();

	bool run();
	bool run_noblock();
};

#endif // _RAYTRACER_CLIENT_H_