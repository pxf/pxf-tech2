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

