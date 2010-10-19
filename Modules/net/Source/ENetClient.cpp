#include <Pxf/Modules/net/ENetClient.h>

using namespace Pxf::Modules;
using namespace Pxf;

bool ENetClient::Connect(const char* _Host, const int _Port)
{
	ENetEvent event;

	enet_address_set_host(&Address, _Host);
	Address.port = _Port;

	Client = enet_host_create(NULL, 1, NetDev->GetTags()->size(), 0, 0);

	if (Client == NULL)
	{
		Message("ENetClient", "No available peers for an ENet connection.");
		return false;
	}
	
#if COMPRESSION == 1
	enet_host_compress_with_range_coder(Client);
	Message("ENetClient", "Enabling range-coder compression.");
#endif

	Peer = enet_host_connect(Client, &Address, NetDev->GetTags()->size(), 0);

	if (Peer == NULL)
	{
		Message("ENetClient", "Unable to connect to server.");
		return false;
	}

	if (enet_host_service(Client, &event, 5000) > 0 &&
		event.type == ENET_EVENT_TYPE_CONNECT)
	{
//		Message("ENetClient", "Connection established. Client %d.", Ident);
		m_Connected = true;
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
			m_Connected = false;
			return true;
		}
	}
	
	enet_peer_reset(Peer);
	Peer = NULL;

	Message("ENetClient", "Force quit. Sowwy server.");
	m_Connected = false;

	return false;
}

bool ENetClient::Connected()
{
	return m_Connected;
}

Pxf::Network::Packet* ENetClient::Recv()
{
	if (BufferedPackets.size() > 0)
	{
		Network::Packet* bpack = BufferedPackets.front();
		BufferedPackets.erase(BufferedPackets.begin());

		return bpack;
	}

	ENetEvent event;
	//ENetDataPacket* packet;
	Network::Packet* packet;
	int retcode;

//	Message("ENetClient", "Recv()...");

	while ((retcode = enet_host_service(Client, &event, 1000)) >= 0)
	{
		switch(event.type)
		{
		case ENET_EVENT_TYPE_NONE:
//			Message("ENetClient", "Timeout.");
			break;

		case ENET_EVENT_TYPE_RECEIVE:
			Message("ENetClient", "Client %d. Packet received from %s on channel %u. Length %u."
				, Ident, event.peer->data, event.channelID, event.packet->dataLength);
			/*
			if (event.packet->dataLength > MAX_PACKET_SIZE)
			{
				Message("ENetClient", "Packet too large (%u > %d), throwing."
					, event.packet->dataLength, MAX_PACKET_SIZE);
				continue;
			}
			*/

			packet = (Network::Packet*)new ENetDataPacket(
				(char*)event.packet->data
				, (int)event.peer->data
				, (int)event.packet->dataLength
				, (int)event.channelID);
			
			enet_packet_destroy(event.packet);

			return packet;
			break;

		default:
			Message("ENetClient", "Unhandled.");
		}
	}

//	Message("ENetClient", "Recv() stop. %d - %d", event.type, retcode);

	return NULL;
}

Pxf::Network::Packet* ENetClient::RecvNonBlocking(const int _Timeout)
{
	if (BufferedPackets.size() > 0)
	{
		Network::Packet* bpack = BufferedPackets.front();
		BufferedPackets.erase(BufferedPackets.begin());

		return bpack;
	}

	ENetEvent event;
	ENetDataPacket* packet;
	int retcode;
	int stopTime = Platform::GetTime() + _Timeout;

//	Message("ENetClient", "Recv()...");
	
	while ((retcode = enet_host_service(Client, &event, stopTime-Platform::GetTime())) >= 0)
	{
		switch(event.type)
		{
		case ENET_EVENT_TYPE_NONE:
//			Message("ENetClient", "Timeout.");
			return NULL;
			break;

		case ENET_EVENT_TYPE_RECEIVE:
			Message("ENetClient", "Client %d. Packet received from %s on channel %u. Length %u."
				, Ident, event.peer->data, event.channelID, event.packet->dataLength);
			/*
			if (event.packet->dataLength > MAX_PACKET_SIZE)
			{
				Message("ENetClient", "Packet too large (%u > %d), throwing."
					, event.packet->dataLength, MAX_PACKET_SIZE);
				continue;
			}
			*/

			packet = new ENetDataPacket(
				(char*)event.packet->data
				, (int)event.peer->data
				, (int)event.packet->dataLength
				, (int)event.channelID);
			
			enet_packet_destroy(event.packet);

			return (Network::Packet*)packet;
			break;

		default:
			Message("ENetClient", "Unhandled.");
		}
	}

//	Message("ENetClient", "Recv() stop. %d - %d", event.type, retcode);

	return NULL;
}

bool ENetClient::Send(const int _Type, const char* _Buf, const int _Length)
{
	return SendID("und", _Type, _Buf, _Length);

/*	ENetPacket *packet;
	char* NewBuf = new char[6+_Length];

	sprintf(NewBuf, "%c00000000%s\0", 0, _Length, _Buf);

	packet = enet_packet_create(NewBuf, 6+_Length, ENET_PACKET_FLAG_RELIABLE);

	if (packet == NULL)
	{
		Message("ENetServer", "Unable to create packet for sending.");
		return false;
	}

	enet_peer_send(Peer, _Type, packet);

	Flush();

	return true;*/
}

bool ENetClient::SendID(const char* _ID, const int _Type, const char* _Buf, const int _Length)
{
	ENetPacket *packet;
	int IDLength = strlen(_ID);
	char* NewBuf = new char[_Length+IDLength+11];

	sprintf(NewBuf, "%c0000%s0000%s\0", 0, _ID, _Buf);

	memcpy((NewBuf+1), &IDLength, 4);
	memcpy((NewBuf+1+4+IDLength), &_Length, 4);

	packet = enet_packet_create(NewBuf, 11+IDLength+_Length, ENET_PACKET_FLAG_RELIABLE);

	if (packet == NULL)
	{
		Message("ENetServer", "Unable to create packet for sending.");
		return false;
	}

	enet_peer_send(Peer, _Type, packet);

	Flush();
	
	delete []NewBuf;
	
	return true;
}

bool ENetClient::SendPacket(Network::Packet* _Packet)
{
	ENetPacket *packet;

	char* ID = _Packet->GetID();
	char* NewBuf = new char[1+4+strlen(ID)+_Packet->GetLength()];
	char* ptr;

	sprintf(NewBuf, "%c0000%s", 1, ID);
	int IDLength = strlen(ID);
	MemoryCopy(NewBuf+1, &IDLength, 4);

	ptr = (NewBuf+1+4+strlen(ID));

	MemoryCopy(ptr, _Packet->GetData(), _Packet->GetLength());

	packet = enet_packet_create(NewBuf, 1+4+strlen(ID)+_Packet->GetLength(), ENET_PACKET_FLAG_RELIABLE);

	if (packet == NULL)
	{
		Message("ENetClient", "Unable to create packet for sending.");
		return false;
	}

	enet_peer_send(Peer, _Packet->GetTag(), packet);

	Flush();

	delete []NewBuf;

	return true;
}

void ENetClient::Flush()
{
	// Force send the packet. Since *_flush doesn't work, we have to do it this way.
	Network::Packet *rpack = RecvNonBlocking(0);
	if (rpack != NULL)
		BufferedPackets.push_back(rpack);
}

