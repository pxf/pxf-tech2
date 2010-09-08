#include <stdio.h>
#include <string.h>

#include <Pxf/Kernel.h>

using namespace Pxf;


int main(int argv, char *argc[])
{
	Kernel* kernel = Kernel::GetInstance();

	kernel->RegisterModule("net", Pxf::System::SYSTEM_TYPE_NETWORKDEVICE, true);
	kernel->DumpAvailableModules();

	int isserver = (argv > 1 && !strcmp(argc[1], "server"));

	if (isserver)
	{
		return 1;
	}
	else /* client */
	{
		return 1;
	}

    return 0;
}
