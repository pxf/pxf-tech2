#include <stdio.h>
#include <string.h>

#include <Pxf/Kernel.h>

#include <Pxf/Network/NetworkDevice.h>

#include <Pxf/Base/Debug.h>

/* #include <windows.h> */
//#include <unistd.h>


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
		Server* server = netdev->CreateServer();
		Packet* packet;
		server->Bind(5006);
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
		Client* client = netdev->CreateClient();
		Client* client2 = netdev->CreateClient();
		client2->Connect("localhost", 5006);
		client->Connect("localhost", 5006);
//		Sleep(1000);
		for(int i=0;i < 10; i++)
		{
			client->Send(def, "Lol");
			client2->Send(crit, "Lol");

			client->RecvNonBlocking(1000);
			client2->RecvNonBlocking(1000);
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
