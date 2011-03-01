#include <Pxf/Base/Platform.h>
#include <Pxf/Base/Memory.h>
#ifdef CONF_FAMILY_UNIX
#include <sys/time.h> 
#include <sys/types.h>
#include <sys/sysctl.h>
#include <unistd.h>

#include <pthread.h>

using namespace Pxf;

bool Platform::s_TimeInitialized = false;
int64 Platform::m_Frequency = -1;
int64 Platform::m_Timestamp = 0;

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
	struct timeval timeVal;
	gettimeofday(&timeVal, NULL);
	m_Timestamp = (int64)timeVal.tv_sec * (int64)1000000 + (int64)timeVal.tv_usec;
	m_Frequency = 1e-6;
}

int64 Platform::GetTime()
{
	struct timeval timeVal;
	gettimeofday(&timeVal, NULL);
	int64 newTimeStamp = (int64)timeVal.tv_sec * (int64)1000000 + (int64)timeVal.tv_usec;
	return (newTimeStamp - m_Timestamp) / (int64)1000;	
}

int32 Platform::GetNumberOfProcessors()
{
	int n = 1;
	n = (int)sysconf(_SC_NPROCESSORS_ONLN);
	return n;
}

void Platform::ThreadSleep(int32 _ms)
{
	usleep(_ms*1000);
}

void* Platform::ThreadCreate(void (*func)(void *), void *userdata)
{
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, (void *(*)(void*))func, userdata);

	return (void*)thread_id;
}

void Platform::ThreadWait(void* thread)
{
	pthread_join((pthread_t)thread, NULL);
}

void Platform::ThreadYield(void* thread)
{
	sched_yield();
}

Platform::Lock Platform::LockCreate()
{
	pthread_mutex_t* lock = (pthread_mutex_t*)MemoryAllocate(sizeof(pthread_mutex_t));
	pthread_mutex_init(lock, 0);
	return (Platform::Lock)lock;
}

void Platform::LockDestroy(Platform::Lock _Lock)
{
	pthread_mutex_destroy((pthread_mutex_t *)_Lock);
	MemoryFree(_Lock);
}

bool Platform::LockTry(Platform::Lock _Lock)
{
	return pthread_mutex_trylock((pthread_mutex_t*)_Lock);
}

void Platform::LockWait(Platform::Lock _Lock)
{
	pthread_mutex_lock((pthread_mutex_t*)_Lock);
}

void Platform::LockRelease(Platform::Lock _Lock)
{
	pthread_mutex_unlock((pthread_mutex_t*)_Lock);
}


#endif // CONF_FAMILY_UNIX