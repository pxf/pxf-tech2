#include <RaytracerClient.h>
#include <Pxf/Base/Platform.h>

using namespace ZThread;
using namespace Pxf;

RaytracerClient::RaytracerClient()
{

}

RaytracerClient::~RaytracerClient()
{

}

bool RaytracerClient::Run()
{

	int numcpu = Platform::GetNumberOfProcessors();

	// wait for jobs

	// add jobs to queue

	// do <numcpu> jobs taken from queue

	// send back results (combine with above step if possible)

	return false;
}