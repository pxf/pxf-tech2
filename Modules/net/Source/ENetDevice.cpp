#include <Pxf/Modules/net/ENetDevice.h>

#include <iterator>

using namespace Pxf;
using namespace Pxf::Network;
using namespace Pxf::Modules;

ENetDevice::ENetDevice(Pxf::Kernel* _Kernel)
	: Pxf::Network::NetworkDevice(_Kernel, "ENetDevice")
{
	if (enet_initialize() != 0)
		Message("ENetDevice", "Unable to initialize enet.");
	else
		Message("ENetDevice", "enet initialized.");
}

ENetDevice::~ENetDevice()
{
	enet_deinitialize();
}

Server* ENetDevice::CreateServer(const int _Port)
{
	static int ident;
	
	ENetServer* server = new ENetServer(_Port);
	Servers.push_back(server);
	server->Ident = ident++;

	return (Server*)server;
}

Client* ENetDevice::CreateClient(const char* _Host, const int _Port)
{
	static int ident;
	
	ENetClient* client = new ENetClient(_Host, _Port);
	Clients.push_back(client);
	client->Ident = ident++;

	return (Client*)client;
}

Server* ENetDevice::GetServer(const int _ServerIdent)
{
	for (int i=0; i < Servers.size(); i++)
		if (Servers[i]->Ident == _ServerIdent)
			return Servers[i];
}

Client* ENetDevice::GetClient(const int _ClientIdent)
{
	for (int i=0; i < Clients.size(); i++)
		if (Clients[i]->Ident == _ClientIdent)
			return Clients[i];
}

void ENetDevice::KillServer(const int _ServerIdent)
{
	Util::Array<ENetServer*>::iterator iter = Servers.begin();
	
	while (iter != Servers.end())
		if ((*iter)->Ident == _ServerIdent)
		{
			Servers.erase(iter);
			break;
		}
}

void ENetDevice::KillClient(const int _ClientIdent)
{
	Util::Array<ENetClient*>::iterator iter = Clients.begin();
	
	while (iter != Clients.end())
		if ((*iter)->Ident == _ClientIdent)
		{
			Clients.erase(iter);
			break;
		}
}

int ENetDevice::CreateType()
{
	static int typeCounter;
	
	return ++typeCounter;
}
