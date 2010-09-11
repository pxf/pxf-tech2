#include <stdio.h>
#include <string.h>

#include <Pxf/Kernel.h>

#include <Pxf/Network/NetworkDevice.h>

using namespace Pxf;
using namespace Pxf::Network;


int main(int argv, char *argc[])
{
	Kernel* kernel = Kernel::GetInstance();

	kernel->RegisterModule("net", Pxf::System::SYSTEM_TYPE_NETWORKDEVICE, true);
	kernel->DumpAvailableModules();

	NetworkDevice* netdev = kernel->GetNetworkDevice();

	int isserver = (argv > 1 && !strcmp(argc[1], "server"));

	if (isserver)
	{
		Server* server = netdev->CreateServer(5006);
		Packet* packet;
		server->Bind();
		while (1)
		{
			packet = server->Recv();
			server->SendAll("hejsan", strlen("hejsan"));
		}
		
		return 1;
	}
	else /* client */
	{
		Client* client = netdev->CreateClient("localhost", 5006);
		client->Connect();
		client->Send("Lol", 3);
		client->Send("Lol, packet 2", strlen("Lol, packet 2"));

		return 1;
	}

    return 0;
}
