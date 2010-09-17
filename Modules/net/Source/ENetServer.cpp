#include <Pxf/Modules/net/ENetServer.h>

using namespace Pxf::Modules;
using namespace Pxf::Util;

int ENetServer::CreateClientID()
{
	static int num;

	return num++;
}

bool ENetServer::Bind(const int _Port)
{
	// TODO: Replace 32 with a settable setting. Maybe add it as an argument to ENetServer?
	// TOOD: Make use of the different channels.
	Address.host = ENET_HOST_ANY;
	Address.port = _Port;

	Message("ENetServer", "Ident %d", Ident);
	Server = enet_host_create(&Address, 32, 2, 0, 0);

#if COMPRESSION == 1
	enet_host_compress_with_range_coder(Server);
	Message("ENetServer", "Enabling range-coder compression.");
#endif

	if (Server == NULL)
	{
		Message("ENetServer", "Could not bind to localhost:%d", Address.port);
		return false;
	}

	return true;
}

bool ENetServer::Shutdown()
{
	return false;
	// TODO:
}

Pxf::Network::Packet* ENetServer::Recv()
{
	ENetEvent event;

	/* Loop until we hit an error. */
	while (enet_host_service(Server, &event, 1000) >= 0)
	{
		switch(event.type)
		{
		case ENET_EVENT_TYPE_NONE:
			Message("ENetServer", "Timeout.");
			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			Message("ENetServer", "Client disconnected: %d.", (int)event.peer->data);
			break;

		case ENET_EVENT_TYPE_CONNECT:
			Message("ENetServer", "Client connected: %x:%u."
				, event.peer->address.host, event.peer->address.port);
			event.peer->data = (void*)CreateClientID();
			break;

		case ENET_EVENT_TYPE_RECEIVE:
			Message("ENetServer", "Packet received from %d on channel %u. Length %u."
				, (int)event.peer->data, event.channelID, event.packet->dataLength);
			if (event.packet->dataLength > MAX_PACKET_SIZE)
			{
				Message("ENetServer", "Packet too large (%u > %d), throwing."
					, event.packet->dataLength, MAX_PACKET_SIZE);
				continue;
			}

			ENetDataPacket* packet = new ENetDataPacket(
				(char*)event.packet->data
				, (int)event.peer->data
				, (int)event.packet->dataLength);

			enet_packet_destroy(event.packet);

			return (Network::Packet*)packet;
		}
	}

	// Error
	return NULL;
}

Pxf::Network::Packet* ENetServer::RecvNonBlocking(const int _Timeout)
{
	ENetEvent event;
	int stopTime = Platform::GetTime() + _Timeout;

	/* Loop until we hit an error. */
	while (enet_host_service(Server, &event, stopTime - Platform::GetTime()) >= 0)
	{
		switch(event.type)
		{
		case ENET_EVENT_TYPE_NONE:
			Message("ENetServer", "Timeout.");
			return NULL;
			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			Message("ENetServer", "Client disconnected: %d.", (int)event.peer->data);
			break;

		case ENET_EVENT_TYPE_CONNECT:
			Message("ENetServer", "Client connected: %x:%u."
				, event.peer->address.host, event.peer->address.port);
			event.peer->data = (void*)CreateClientID();
			break;

		case ENET_EVENT_TYPE_RECEIVE:
			Message("ENetServer", "Packet received from %d on channel %u. Length %u."
				, (int)event.peer->data, event.channelID, event.packet->dataLength);
			if (event.packet->dataLength > MAX_PACKET_SIZE)
			{
				Message("ENetServer", "Packet too large (%u > %d), throwing."
					, event.packet->dataLength, MAX_PACKET_SIZE);
				continue;
			}

			ENetDataPacket* packet = new ENetDataPacket(
				(char*)event.packet->data
				, (int)event.peer->data
				, (int)event.packet->dataLength);

			enet_packet_destroy(event.packet);

			return (Network::Packet*)packet;
		}
	}

	// Error
	return NULL;
}

bool ENetServer::Send(const int _Client, const int _Type, const char* _Buf)
{
	ENetPacket *packet;

	packet = enet_packet_create(_Buf, strlen(_Buf)+1, ENET_PACKET_FLAG_RELIABLE);

	enet_peer_send(&Server->peers[_Client], _Type, packet);
	enet_packet_destroy(packet);
	enet_host_flush(Server);

	return true;
}

bool ENetServer::SendAll(const int _Type, const char* _Buf)
{
	ENetPacket *packet;
	ENetPeer *peer;

	packet = enet_packet_create(_Buf, strlen(_Buf)+1, ENET_PACKET_FLAG_RELIABLE);

	enet_host_broadcast(Server, _Type, packet);


	// TODO: Some way of destroying this packet.
	//		 Even better, is it destroyed by the enet-library?
//	enet_packet_destroy(packet);
//	enet_host_flush(Server);

	return true;
}
