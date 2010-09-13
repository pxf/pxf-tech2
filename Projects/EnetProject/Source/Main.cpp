#include <stdio.h>
#include <string.h>

#include <Pxf/Kernel.h>

#include <Pxf/Network/NetworkDevice.h>

#include <Pxf/Base/Debug.h>

/* #include <windows.h> */
#include <unistd.h>


using namespace Pxf;
using namespace Pxf::Network;


int main(int argv, char *argc[])
{
	Kernel* kernel = Kernel::GetInstance();

	kernel->RegisterModule("net", Pxf::System::SYSTEM_TYPE_NETWORKDEVICE, true);
	kernel->DumpAvailableModules();

	NetworkDevice* netdev = kernel->GetNetworkDevice();
	
	int def = netdev->CreateType();
	int crit = netdev->CreateType();

	int isserver = (argv > 1 && !strcmp(argc[1], "server"));

	if (isserver)
	{
		Server* server = netdev->CreateServer(5006);
		Packet* packet;
		server->Bind();
		while (1)
		{
			packet = server->Recv();
			Message("Main Server", "Got packet \"%s\"", packet->GetData());
			//sleep(1);
			server->SendAll(def, "lol");
			Message("Main", "SendAll");
		}
		
		return 1;
	}
	else /* client */
	{
		Client* client = netdev->CreateClient("localhost", 5006);
		Client* client2 = netdev->CreateClient("localhost", 5006);
		client2->Connect();
		client->Connect();
//		Sleep(1000);
		for(int i=0;i < 10; i++)
		{
			client->Send(def, "Lol");
			client2->Send(crit, "Lol");

			client->Recv();
			client2->Recv();
		}
//		Packet* packet = client->Recv();
//		Sleep(1000);
//		Message("Main Client", "Got packet \"%s\"", packet->GetData());
		client->Disconnect();
		client2->Disconnect();

		return 1;
	}

    return 0;
}
