#ifndef _RAYTRACER_CLIENT_H_
#define _RAYTRACER_CLIENT_H_

#include <Pxf/Base/Types.h>
#include <zthread/LockedQueue.h>
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


class RenderJob
{
private:
	Rect m_ScreenRect;
	uint32 m_RenderTime;
	uint8* m_Pixels;
public:
	RenderJob()
		: m_RenderTime(0)
		, m_Pixels(0)
	{}
};

class RaytracerClient
{
private:
	ZThread::LockedQueue<RenderJob, ZThread::FastMutex> m_RenderQueue;
public:
	RaytracerClient();
	~RaytracerClient();

	bool Run();
};

#endif // _RAYTRACER_CLIENT_H_