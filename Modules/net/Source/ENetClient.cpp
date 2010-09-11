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
	ENetEvent event;

	enet_peer_disconnect(Peer, 0);

	while (enet_host_service(Client, &event, 5000) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_RECEIVE:
			enet_packet_destroy(event.packet);
			break;
			
		case ENET_EVENT_TYPE_DISCONNECT:
			Message("ENetClient", "Disconnected successfully.");
			return true;
		}
	}
	
	enet_peer_reset(Peer);
	Peer = NULL;

	Message("ENetClient", "Force quit. Sowwy server.");

	return false;
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

	enet_packet_destroy(packet);
	enet_host_flush(Client);

	return true;
}
