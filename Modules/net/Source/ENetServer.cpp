#include <Pxf/Modules/net/ENetServer.h>

using namespace Pxf::Modules;


ENetServer::ENetServer(const int _Port)
{
	Address.host = ENET_HOST_ANY;
	Address.port = _Port;
}

bool ENetServer::Bind()
{
	// TODO: Replace 32 with a settable setting. Maybe add it as an argument to ENetServer?
	// TOOD: Make use of the different channels.
	Server = enet_host_create(&Address, 32, 2, 0, 0);

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
			Message("ENetServer", "Client disconnected: %s.", event.peer->data);
			break;

		case ENET_EVENT_TYPE_CONNECT:
			Message("ENetServer", "Client connected: %x:%u."
				, event.peer->address.host, event.peer->address.port);
			break;

		case ENET_EVENT_TYPE_RECEIVE:
			Message("ENetServer", "Packet received from %s on channel %u. Length %u."
				, event.peer->data, event.channelID, event.packet->dataLength);
			if (event.packet->dataLength > MAX_PACKET_SIZE)
			{
				Message("ENetServer", "Packet too large (%u > %d), throwing."
					, event.packet->dataLength, MAX_PACKET_SIZE);
				continue;
			}

			LPLength = event.packet->dataLength;
			LPSource = (int)event.peer->data;
			LPChannel = (int)event.channelID;

			ENetDataPacket* packet = new ENetDataPacket((char*)event.packet->data, (int)event.peer->data, (int)event.packet->dataLength);

			enet_packet_destroy(event.packet);

			return (Network::Packet*)packet;
		}
	}

	// Error
	return NULL;
}

bool ENetServer::Send(const int _Client, const char* _Buf, const int _Length)
{
	ENetPacket *packet;

	packet = enet_packet_create(_Buf, _Length+1, ENET_PACKET_FLAG_RELIABLE);

//	enet_peer_send(, 0

	return true;
}

bool ENetServer::SendAll(const char* _Buf, const int _Length)
{
	ENetPacket *packet;
	ENetPeer *peer;

	packet = enet_packet_create(_Buf, _Length+1, ENET_PACKET_FLAG_RELIABLE);

	for(int i = 0; i < Clients.size(); i++)
	{
		enet_peer_send(Clients[i], 0, packet);
	}

	enet_packet_destroy(packet);
	enet_host_flush(Server);

	return true;
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

