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


class TaskRequest
{
public:
	Rect rect;
};

class TaskResult
{
	uint8* pixels;
};

typedef ZThread::BlockingQueue<TaskRequest*, ZThread::FastMutex> TaskRequestQueue;
typedef ZThread::BlockingQueue<TaskResult*, ZThread::FastMutex> TaskResultQueue;

class LightningClient
{
protected:
	TaskRequestQueue m_queue_in;
	TaskResultQueue m_queue_out;
public:
	TaskRequest* get_request()
	{
		return m_queue_in.next();
	}

	void put_result(TaskResult* _Result)
	{
		m_queue_out.add(_Result);
	}
};


/*
class LightningClient
{
protected:
	BlockingArray arr;
	Condition hastasks[numtypes];
public:
	TaskRequest* get_request(tasktype)
	{
		// conditions borde vara i BlockingArray etc etc
		if (inte tom)
			hastasks[tasktype].wait();

		arr.acquire();
		//arr.find_task_of_type(RAYTRACE);
		arr.get_task(task_parent)
		arr.release();
	}

	void put_result(TaskResult* _Result)
	{
		// queue borde funka fint
		// men kanske borde wrappas med typ osv
		m_queue_out.add(_Result);
	}
}

*/


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