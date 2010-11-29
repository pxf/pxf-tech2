#ifndef _RAYTRACER_CLIENT_H_
#define _RAYTRACER_CLIENT_H_

#include <Pxf/Base/Types.h>
#include <Pxf/Kernel.h>

// todo: forward-declare these and use pointers to minimize header dependencies.
#include <zthread/PoolExecutor.h>
#include <zthread/BlockingQueue.h>
#include <zthread/FastMutex.h>

#include <BlockingTaskQueue.h>

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

typedef ZThread::BlockingQueue<TaskResult*, ZThread::FastMutex> TaskResultQueue;

class LightningClient
{
protected:
	BlockingTaskQueue<TaskRequest*> m_TaskQueue;
	TaskResultQueue m_ResultQueue;
public:
	enum TaskType {RayTraceTask, HerpaDerpTask};
	virtual TaskRequest* get_request() = 0;
	virtual void put_result(TaskResult* _Result) = 0;
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

	virtual TaskRequest* get_request();
	virtual void put_result(TaskResult* _Result);

	bool run();
	bool run_noblock();
};

#endif // _RAYTRACER_CLIENT_H_