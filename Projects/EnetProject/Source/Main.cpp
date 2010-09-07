#include <stdio.h>
#include <string.h>

#include <Pxf/Kernel.h>

#include <Pxf/Modules/net/ENetServer.h>
#include <Pxf/Modules/net/ENetClient.h>

using namespace Pxf;
using namespace Pxf::Modules;


int main(int argv, char *argc[])
{
	Kernel* kernel = Kernel::GetInstance();
	kernel->RegisterModule("net", Pxf::System::SYSTEM_TYPE_NETWORKDEVICE, true);

	int isserver = (argv > 1 && !strcmp(argc[1], "server"));

	if (isserver)
	{
		ENetServer server = Pxf::Modules::ENetServer(kernel);
	}
	else /* client */
	{
		return 1;
	}

    return 0;
}
