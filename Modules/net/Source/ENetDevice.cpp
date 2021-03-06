#include <Pxf/Modules/net/ENetDevice.h>
#include <Pxf/Base/Memory.h>

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

	ChildID = 0;
}

ENetDevice::~ENetDevice()
{
	enet_deinitialize();
}

Server* ENetDevice::CreateServer()
{
	ENetServer* server = new ENetServer(this);
	server->Ident = ChildID++;

	Servers.push_back(server);
/*	for (int i=0;i<Servers.capacity();i++)
		if (Servers[i] == NULL)
			Servers[i] = server;*/

	return (Server*)server;
}

Client* ENetDevice::CreateClient()
{
	ENetClient* client = new ENetClient(this);
	client->Ident = ChildID++;

	Clients.push_back(client);
/*	for (int i=0;i<Clients.capacity();i++)
		if (Clients[i] == NULL)
			Clients[i] = client;*/

	return (Client*)client;
}

Server* ENetDevice::GetServer(const int _ServerIdent)
{
	for (int i=0; i < Servers.size(); i++)
		if (Servers[i] != NULL && Servers[i]->Ident == _ServerIdent)
			return Servers[i];
	
	return NULL;
}

Client* ENetDevice::GetClient(const int _ClientIdent)
{
	for (int i=0; i < Clients.size(); i++)
		if (Clients[i] != NULL && Clients[i]->Ident == _ClientIdent)
			return Clients[i];
	
	return NULL;
}

Util::Array<Network::Client*>& ENetDevice::GetClients()
{
	return Clients;
}

Util::Array<Network::Server*>& ENetDevice::GetServers()
{
	return Servers;
}

// TODO: These should also kill the actual connections that are open.
// TODO: Maybe add a ForceKillServer/Client ?
void ENetDevice::KillServer(const int _ServerIdent)
{
	Util::Array<Network::Server*>::iterator iter = Servers.begin();
	
	while (iter != Servers.end())
		if ((*iter)->Ident == _ServerIdent)
		{
			iter = Servers.erase(iter);
			break;
		}
}

void ENetDevice::KillClient(const int _ClientIdent)
{
	Util::Array<Network::Client*>::iterator iter = Clients.begin();
	
	while (iter != Clients.end())
		if ((*iter)->Ident == _ClientIdent)
		{
			iter = Clients.erase(iter);
			break;
		}
}

int ENetDevice::AddTag(char* _Name)
{
	char* tmp = new char[strlen(_Name)+1];
	MemoryCopy(tmp, _Name, strlen(_Name));
//	strcpy(tmp, _Name);
	Tags.push_back(tmp);

	return Tags.size()-1;
}

Util::Array<char*>* ENetDevice::GetTags()
{
	return &Tags;
}

Packet* ENetDevice::CreateEmptyPacket(const char* _ID, const int _Tag)
{
	Packet* packet = new ENetDataPacket(_ID, 0, _Tag);

	return packet;
}

Packet* ENetDevice::CreatePacket(char* _Data, const int _Tag, const int _Length, const int _Sender)
{
	Packet* packet = new ENetDataPacket(_Data, _Sender, _Length, _Tag);

	return packet;
}

int ENetDevice::NumClients()
{
	return Clients.size();
}
