#include <Pxf/Base/Platform.h>
#ifdef CONF_FAMILY_UNIX
#include <sys/time.h> 
#include <sys/types.h>
#include <sys/sysctl.h>
#include <unistd.h>

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

#endif // CONF_FAMILY_UNIX