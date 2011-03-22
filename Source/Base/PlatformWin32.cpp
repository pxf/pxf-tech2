#include <Pxf/Base/Platform.h>
#include <Pxf/Base/Memory.h>
#ifdef CONF_FAMILY_WINDOWS

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h> // QueryPerformanceCounter et al

using namespace Pxf;

bool Platform::s_TimeInitialized = false;
int64 Platform::m_Timestamp = 0;
int64 Platform::m_Frequency = -1;

class PlatformInitializer
{
public:
	PlatformInitializer()
	{
		Platform platform;
	}
};
static PlatformInitializer init;

void Platform::Initialize()
{
	int64 freq = 0;
	if (QueryPerformanceFrequency((LARGE_INTEGER*) &freq))
	{
		m_Frequency = freq;
		QueryPerformanceCounter((LARGE_INTEGER*) &m_Timestamp);
	}
	else
	{
		m_Frequency = -1;
		// TODO:  fallback on timeGetTime()?
	}
}

int64 Platform::GetTime()
{
	int64 newTimeStamp;
	QueryPerformanceCounter((LARGE_INTEGER*) &newTimeStamp);
	
	return ((newTimeStamp - m_Timestamp) * 1000) / m_Frequency;	
}

int32 Platform::GetNumberOfProcessors()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	return si.dwNumberOfProcessors;
}

void Platform::ThreadSleep(int32 _ms)
{
	Sleep(_ms);
}

void Platform::ThreadYield()
{
	Sleep(0);
}

void* Platform::ThreadCreate(void (*func)(void *), void *userdata)
{
	return CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, userdata, 0, NULL);
}

void Platform::ThreadWait(void* thread)
{
	WaitForSingleObject((HANDLE)thread, INFINITE);
}

Platform::Lock Platform::LockCreate()
{
	CRITICAL_SECTION* lock = (CRITICAL_SECTION*)MemoryAllocate(sizeof(CRITICAL_SECTION));
	InitializeCriticalSection((LPCRITICAL_SECTION)lock);
	return (Platform::Lock)lock;
}

void Platform::LockDestroy(Platform::Lock _Lock)
{
	DeleteCriticalSection((LPCRITICAL_SECTION)_Lock);
	MemoryFree(_Lock);
}

bool Platform::LockTry(Platform::Lock _Lock)
{
	return TryEnterCriticalSection((LPCRITICAL_SECTION)_Lock);
}

void Platform::LockWait(Platform::Lock _Lock)
{
	EnterCriticalSection((LPCRITICAL_SECTION)_Lock);
}

void Platform::LockRelease(Platform::Lock _Lock)
{
	LeaveCriticalSection((LPCRITICAL_SECTION)_Lock);
}

#endif // CONF_FAMILY_WINDOWS