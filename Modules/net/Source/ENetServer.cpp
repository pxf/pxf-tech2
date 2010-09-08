#include <Pxf/Modules/net/ENetServer.h>

using namespace Pxf::Modules;

ENetServer::ENetServer()
{
}

bool ENetServer::Bind()
{
	return false;
}
bool ENetServer::Shutdown()
{
	return false;
}

int ENetServer::Recv(char* _Buf)
{
	return false;
}
bool ENetServer::Send(const int _Client, const char* _Buf, const int _Length)
{
	return false;
}
bool ENetServer::SendAll(const char* _Buf, const int _Length)
{
	return false;
}

/*
#include <Pxf/Modules/net/ENetServer.h>

using namespace Pxf::Modules;

ENetServer::ENetServer(Pxf::Kernel* _Kernel)
	: Pxf::Network::NetworkDevice(_Kernel, "ENetServer Device")
{
	if (enet_initialize() != 0)
		Message("ENetServer", "Unable to initialize enet.");
	atexit(enet_deinitialize);
}

bool ENetServer::Recv(char* _Buf)
{
	ENetEvent event;

	while (enet_host_service(Server, &event, 1000) >= 0)
	{
		switch (event.type)
		{
			case ENET_EVENT_TYPE_RECEIVE:
				Message("ENetServer", "Packet.");
				break;

			case ENET_EVENT_TYPE_CONNECT:
				Message("ENetServer", "Client connected: %x:%u.", event.peer->address.host, event.peer->address.port);
				break;
			
			case ENET_EVENT_TYPE_DISCONNECT:
				Message("ENetServer", "Client DISconnected: %s.", event.peer->data);
				break;

			case ENET_EVENT_TYPE_NONE:
				Message("ENetServer", "Timeout.");
				break;
		}
	}

	Message("ENetServer", "Couldn't fetch event.");
	return false;
}

bool ENetServer::Send(const char* _Buf, const int _Length)
{
	return true;
}

bool ENetServer::Bind(const int _Port)
{
	Address.host = ENET_HOST_ANY; // TODO: Change this to something dynamic.
	Address.port = _Port;

	Server = enet_host_create(&Address, 5, 2, 0, 0);
	if (Server == NULL)
	{
		Message("ENetServer", "Unable to bind.");
		return false;
	}

	return true;
}
*/

