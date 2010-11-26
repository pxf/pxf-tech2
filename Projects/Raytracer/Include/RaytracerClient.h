#ifndef _RAYTRACER_CLIENT_H_
#define _RAYTRACER_CLIENT_H_

#include <Pxf/Base/Types.h>
#include <Pxf/Kernel.h>
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
	JobRequestQueue m_InQueue;
	JobResultQueue m_OutQueue;
public:
	JobRequest* GetRequest()
	{
		return m_InQueue.next();
	}

	void PutResult(JobResult* _Result)
	{
		m_OutQueue.add(_Result);
	}
};

class RaytracerClient : public LightningClient
{
private:
	Pxf::Kernel* m_Kernel;
public:
	RaytracerClient(Pxf::Kernel* _Kernel);
	~RaytracerClient();

	bool Run();
};

#endif // _RAYTRACER_CLIENT_H_