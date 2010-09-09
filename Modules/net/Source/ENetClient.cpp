#include <Pxf/Modules/net/ENetClient.h>

using namespace Pxf::Modules;

ENetClient::ENetClient(const char* _Host, const int _Port)
{
	enet_address_set_host(&Address, _Host);
	Address.port = _Port;
}

bool ENetClient::Connect()
{
	ENetEvent event;

	Client = enet_host_create(NULL, 1, 2, 0, 0);

	if (Client == NULL)
	{
		Message("ENetClient", "No available peers for an ENet connection.");
		return false;
	}

	Peer = enet_host_connect(Client, &Address, 2, 0);

	if (Peer == NULL)
	{
		Message("ENetClient", "Unable to connect to server.");
		return false;
	}

	if (enet_host_service(Client, &event, 5000) > 0 &&
		event.type == ENET_EVENT_TYPE_CONNECT)
	{
		Message("ENetClient", "Connection established.");
		return true;
	}
	else
	{
		Message("ENetClient", "Unable to connect.");
		return false;
	}
}

bool ENetClient::Disconnect()
{
	/* TODO: Tell the server we've decided to quit. UDP biatch. */
	enet_host_destroy(Client);

	return true;
}

bool ENetClient::Connected()
{
	return true;
}

int ENetClient::Recv(char* _Buf)
{
	return 0;
}

// TODO: Add support for different priorities.
// TODO: Add support for use of different channels.
bool ENetClient::Send(const char* _Buf, const int _Length)
{
	ENetPacket *packet;

	packet = enet_packet_create(_Buf, _Length+1, ENET_PACKET_FLAG_RELIABLE);

	// Send over channel 0.
	enet_peer_send(Peer, 0, packet);

	enet_host_flush(Client);

	return true;
}

/*#include <Pxf/Modules/net/ENetClient.h>

using namespace Pxf::Modules;

ENetClient::ENetClient(Pxf::Kernel* _Kernel)
	: Pxf::Network::NetworkDevice(_Kernel, "ENetClient Device")
{
	if (enet_initialize() != 0)
		Message("ENetClient", "Unable to initialize enet.");
	atexit(enet_deinitialize);
}

bool ENetClient::Connected()
{
	return false; // TODO:
}

bool ENetClient::Recv(char* _Buf)
{
	ENetEvent event;

	while (enet_host_service(Client, &event, 1000) >= 0)
	{
		switch (event.type)
		{
			case ENET_EVENT_TYPE_RECEIVE:
				Message("ENetClient", "Packet.");
				break;

			case ENET_EVENT_TYPE_CONNECT:
				Message("ENetClient", "Client connected: %x:%u.", event.peer->address.host, event.peer->address.port);
				break;
			
			case ENET_EVENT_TYPE_DISCONNECT:
				Message("ENetClient", "Client DISconnected: %s.", event.peer->data);
				break;

			case ENET_EVENT_TYPE_NONE:
				Message("ENetClient", "Timeout.");
				break;
		}
	}

	Message("ENetClient", "Couldn't fetch event.");
	return false;
}

bool ENetClient::Send(const char* _Buf, const int _Length)
{
	ENetPacket *packet = enet_packet_create(_Buf, _Length+1, ENET_PACKET_FLAG_RELIABLE);

//	enet_peer_send();
}

bool ENetClient::Connect(const char* _Host, const int _Port)
{
	enet_address_set_host(&Address, _Host);
	Address.port = _Port;

	Client = enet_host_create(NULL, 1, 2, 0, 0);

	if (Client == NULL)
		Message("ENetClient", "Unable to create ENet client host.");
	
	//enet_host_destroy();
	
	Peer = enet_host_connect(Client, &Address, 2, 0);
	if (Peer == NULL)
		Message("ENetClient", "Unable to connect to given host name.");
}

bool ENetClient::Disconnect()
{
	return false;
}*/
