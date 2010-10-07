#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Timer.h>
#include <Pxf/Base/Logger.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Random.h>
#include <Pxf/Base/Memory.h>

#include <Pxf/Network/NetworkDevice.h>


#include <windows.h>

#include <ctime>

using namespace Pxf;

int main(int argc, char* argv[])
{
	Pxf::RandSetSeed(time(NULL));
	Kernel* kernel = Pxf::Kernel::GetInstance();
	kernel->RegisterModule("net", 0xFFFF, true);
	kernel->DumpAvailableModules();
	int nettag = kernel->CreateTag("Net");

	Network::NetworkDevice* net = kernel->GetNetworkDevice();
	int tag1 = net->AddTag("tag1");
	int tag2 = net->AddTag("tag2");
	int tag3 = net->AddTag("tag3");

	/* Client */
	if (argc > 1 && StringCompareI(argv[1], "client") == 0)
	{
		kernel->Log(nettag | Logger::IS_DEBUG, "Starting client");
		Network::Client* client = net->CreateClient();
		client->Connect("127.0.0.1", 9001);

		while (true)
		{
			const char data[] = "herpa-derp-di-splerp";
			client->SendID("aoe", tag1, data, sizeof(data));
			client->SendID("aoe", tag2, data, sizeof(data));
			client->SendID("aoe", tag3, data, sizeof(data));
			client->Recv();
			Sleep(500);
			//Network::Packet* packet = client->Recv(4096);
			//kernel->Log(nettag | Logger::IS_CRITICAL, "Tag: %d, Id: %s", packet->GetTag(), packet->GetID());
		}
	}
	/* Server */
	else if (argc > 1 && StringCompareI(argv[1], "server") == 0)
	{
		kernel->Log(nettag | Logger::IS_DEBUG, "Starting server");
		Network::Server* server = net->CreateServer();
		server->Bind(9001);

		while (true)
		{
			Network::Packet* packet = server->Recv();
			kernel->Log(nettag | Logger::IS_DEBUG, "Tag: %d, Id: %s", packet->GetTag(), packet->GetID());
			const char data[] = "herpa-derp-di-splerp";
			server->SendAllID("aoe", tag1, data, sizeof(data));
		}
	}
	return 0;
}

