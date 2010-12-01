#include <Pxf/Base/Timer.h>
#include <Pxf/Base/Platform.h>

using namespace Pxf;

Timer::Timer()
{
	// make sure it Platform is initialized
	Platform p;
	m_Start = 0;
	m_Stop = 0;
	m_Stopped = false;
}

void Timer::Start()
{
	m_Stopped = false;
	m_Start = Platform::GetTime();
}


void Timer::Stop()
{
	m_Stop = Platform::GetTime();
	m_Stopped = true;
}

uint64 Timer::Interval()
{
	uint64 tmp;
	if (m_Stopped)
		tmp = m_Stop;
	else
		tmp = Platform::GetTime();
	return tmp - m_Start;
}
