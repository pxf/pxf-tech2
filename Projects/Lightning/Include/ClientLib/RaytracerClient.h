#ifndef _RAYTRACER_CLIENT_H_
#define _RAYTRACER_CLIENT_H_

#include <Pxf/Base/Types.h>
#include <Pxf/Kernel.h>

// todo: forward-declare these and use pointers to minimize header dependencies.
#include <zthread/PoolExecutor.h>
#include <zthread/BlockingQueue.h>
#include <zthread/FastMutex.h>

#include <BlockingTaskQueue.h>

#include "raytracer.pb.h"

#include <lightning.h>

struct batch_blob_t;

/*
struct Rect_t
{
	int x, y;
	int w, h;

	Rect_t()
		: x(0), y(0)
		, w(0), h(0)
	{}
};

struct TaskRequest
{
	Rect_t rect;
	batch_blob_t* blob;
};

struct TaskResult
{
	Rect_t rect;
	uint8* pixels;
	bool final;
};
*/
class LightningClient
{
protected:
	BlockingTaskQueue<Task*> m_TaskQueue;
	
	typedef ZThread::BlockingQueue<TaskResult*, ZThread::FastMutex> TaskResultQueue;
	TaskResultQueue m_ResultQueue;

	enum TaskType {RayTraceTask, HerpaDerpTask};
public:
	virtual void push_request(Task* _Request) = 0;
	virtual Task* pop_request() = 0;
	virtual void push_result(TaskResult* _Result) = 0;
	virtual TaskResult* pop_result() = 0;
};


class RaytracerClient : public LightningClient
{
protected:
	Pxf::Kernel* m_Kernel;
	ZThread::PoolExecutor* m_Executor;
	unsigned m_NumWorkers;
	unsigned m_LogTag;
public:
	typedef ZThread::BlockingQueue<TaskResult*, ZThread::FastMutex> TaskResultQueue;
	RaytracerClient(Pxf::Kernel* _Kernel, BlockingTaskQueue<Task*>* _TaskQueue, ZThread::BlockingQueue<TaskResult*, ZThread::FastMutex>* _ResultQueue);
	~RaytracerClient();

	virtual void push_request(Task* _Request);
	virtual Task* pop_request();
	virtual void push_result(TaskResult* _Result);
	virtual TaskResult* pop_result();
	bool has_results();

	bool run();
	bool run_noblock();
	bool wait();

	void cancel();
	void interrupt();
};

#endif // _RAYTRACER_CLIENT_H_
