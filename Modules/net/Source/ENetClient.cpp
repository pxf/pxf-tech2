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
	
#if COMPRESSION == 1
	enet_host_compress_with_range_coder(Client);
	Message("ENetClient", "Enabling range-coder compression.");
#endif

	Peer = enet_host_connect(Client, &Address, 2, 0);

	if (Peer == NULL)
	{
		Message("ENetClient", "Unable to connect to server.");
		return false;
	}

	if (enet_host_service(Client, &event, 5000) > 0 &&
		event.type == ENET_EVENT_TYPE_CONNECT)
	{
		Message("ENetClient", "Connection established. Client %d.", Ident);
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

Pxf::Network::Packet* ENetClient::Recv()
{
	ENetEvent event;
	ENetDataPacket* packet;
	int retcode;

	Message("ENetClient", "Recv()...");

	while ((retcode = enet_host_service(Client, &event, 1000)) >= 0)
	{
		switch(event.type)
		{
		case ENET_EVENT_TYPE_NONE:
			Message("ENetClient", "Timeout.");
			break;

		case ENET_EVENT_TYPE_RECEIVE:
			Message("ENetClient", "Client %d. Packet received from %s on channel %u. Length %u."
				, Ident, event.peer->data, event.channelID, event.packet->dataLength);
			if (event.packet->dataLength > MAX_PACKET_SIZE)
			{
				Message("ENetClient", "Packet too large (%u > %d), throwing."
					, event.packet->dataLength, MAX_PACKET_SIZE);
				continue;
			}

			packet = new ENetDataPacket(
				(char*)event.packet->data
				, (int)event.peer->data
				, (int)event.packet->dataLength);
			
			enet_packet_destroy(event.packet);

			return (Network::Packet*)packet;
			break;

		default:
			Message("ENetClient", "Unhandled.");
		}
	}

	Message("ENetClient", "Recv() stop. %d - %d", event.type, retcode);

	return NULL;
}

// TODO: Add support for different priorities.
// TODO: Add support for use of different channels.
bool ENetClient::Send(const int _Type, const char* _Buf)
{
	ENetPacket *packet;

	packet = enet_packet_create(_Buf, strlen(_Buf)+1, ENET_PACKET_FLAG_RELIABLE);

	// Send over channel 0.
	enet_peer_send(Peer, _Type, packet);

	// TODO: Need a way of destroying the packets.
//	enet_host_flush(Client);
//	enet_packet_destroy(packet);

	return true;
}
