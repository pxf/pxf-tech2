#include <Pxf/Modules/net/ENetDevice.h>

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
	return NULL;
}

Client* ENetDevice::CreateClient(const char* _Host, const int _Port)
{
	return NULL;
}

Server* ENetDevice::GetServer(const int _ServerIdent)
{
	return NULL;
}

Client* ENetDevice::GetClient(const int _ClientIdent)
{
	return NULL;
}

void ENetDevice::KillServer(const int _ServerIdent)
{
	return;
}

void ENetDevice::KillClient(const int _ClientIdent)
{
	return;
}
