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

/* Recv()
** Fills _Buf with the fetched packet.
** Returns length of the packet. 0 means error.
**/
int ENetServer::Recv(char* _Buf)
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

			sprintf(_Buf, "%s\0", event.packet->data);
			sprintf(LPData, "%s\0", event.packet->data); // Unnecessary?

			LPLength = event.packet->dataLength;
			LPSource = (int)event.peer->data;
			LPChannel = (int)event.channelID;

			enet_packet_destroy(event.packet);

			return event.packet->dataLength;
			break;
		}
	}

	// Error
	return -1;
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

