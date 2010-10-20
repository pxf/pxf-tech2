#include <Pxf/Modules/net/ENetServer.h>


using namespace Pxf::Modules;
using namespace Pxf::Util;
using namespace Pxf;

int ENetServer::CreateClientID()
{
	static int num;

	return num++;
}

bool ENetServer::Bind(const int _Port)
{
	m_Address.host = ENET_HOST_ANY;
	m_Address.port = _Port;

//	Message("ENetServer", "Ident %d %d", Ident, NetDev->GetTags()->size());
	m_Server = enet_host_create(&m_Address, 1, m_NetDev->GetTags()->size(), 0, 0);

#if COMPRESSION == 1
	enet_host_compress_with_range_coder(Server);
	Message("ENetServer", "Enabling range-coder compression.");
#endif

	if (m_Server == NULL)
	{
		Message("ENetServer", "Could not bind to localhost:%d", m_Address.port);
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
	if (m_BufferedPackets.size() > 0)
	{
		Network::Packet* bpack = m_BufferedPackets.front();
		m_BufferedPackets.erase(m_BufferedPackets.begin());

		return bpack;
	}

	ENetEvent event;

	/* Loop until we hit an error. */
	while (enet_host_service(m_Server, &event, 1000) >= 0)
	{
		switch(event.type)
		{
		case ENET_EVENT_TYPE_NONE:
			Message("ENetServer", "Timeout.");
			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			Message("ENetServer", "Client disconnected: %d.", (int)event.peer->data);
			m_Clients--;
			break;

		case ENET_EVENT_TYPE_CONNECT:
			m_Clients++;
			Message("ENetServer", "Client connected: %x:%u."
				, event.peer->address.host, event.peer->address.port);
			event.peer->data = (void*)CreateClientID();
			break;

		case ENET_EVENT_TYPE_RECEIVE:
			Message("ENetServer", "Packet received from %d on channel %u. Length %u."
				, (int)event.peer->data, event.channelID, event.packet->dataLength);
			/*
			if (event.packet->dataLength > MAX_PACKET_SIZE)
			{
				Message("ENetServer", "Packet too large (%u > %d), throwing."
					, event.packet->dataLength, MAX_PACKET_SIZE);
				continue;
			}
			*/

			ENetDataPacket* packet = new ENetDataPacket(
				(char*)event.packet->data
				, (int)event.peer->data
				, (int)event.packet->dataLength
				, (int)event.channelID);

			enet_packet_destroy(event.packet);

			return (Network::Packet*)packet;
		}
	}

	// Error
	return NULL;
}

Pxf::Network::Packet* ENetServer::RecvNonBlocking(const int _Timeout)
{
	if (m_BufferedPackets.size() > 0)
	{
		Network::Packet* bpack = m_BufferedPackets.front();
		m_BufferedPackets.erase(m_BufferedPackets.begin());

		return bpack;
	}

	ENetEvent event;
	int stopTime = Platform::GetTime() + _Timeout;

	/* Loop until we hit an error or timeout. */
	while (enet_host_service(m_Server, &event, stopTime - Platform::GetTime()) >= 0)
	{
		switch(event.type)
		{
		case ENET_EVENT_TYPE_NONE:
//			Message("ENetServer", "Timeout.");
			return NULL;
			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			Message("ENetServer", "Client disconnected: %d.", (int)event.peer->data);
			m_Clients--;
			break;

		case ENET_EVENT_TYPE_CONNECT:
			Message("ENetServer", "Client connected: %x:%u."
				, event.peer->address.host, event.peer->address.port);
			event.peer->data = (void*)CreateClientID();
			m_Clients++;
			break;

		case ENET_EVENT_TYPE_RECEIVE:
			Message("ENetServer", "Packet received from %d on channel %u. Length %u."
				, (int)event.peer->data, event.channelID, event.packet->dataLength);
			/*
			if (event.packet->dataLength > MAX_PACKET_SIZE)
			{
				Message("ENetServer", "Packet too large (%u > %d), throwing."
					, event.packet->dataLength, MAX_PACKET_SIZE);
				continue;
			}
			*/

			ENetDataPacket* packet = new ENetDataPacket(
				(char*)event.packet->data
				, (int)event.peer->data
				, (int)event.packet->dataLength
				, (int)event.channelID);

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
	char ID[] = "und\0";
	int Length = strlen(_Buf)+1;
	int IDLength = strlen(ID)+1;
	char* NewBuf = new char[Length+IDLength+10];

	sprintf(NewBuf, "%c0000%s0000%s\0", 0, ID, _Buf);

	memcpy((NewBuf+1), &IDLength, 4);
	memcpy((NewBuf+5+IDLength), &Length, 4);

	packet = enet_packet_create(NewBuf, 10+IDLength+Length, ENET_PACKET_FLAG_RELIABLE);

	if (packet == NULL)
	{
		Message("ENetServer", "Unable to create packet for sending.");
		return false;
	}
	enet_peer_send(&m_Server->peers[_Client], _Type, packet);

	Flush();
	
	delete []NewBuf;

	return true;
}

bool ENetServer::SendID(const int _Client, const char* _ID, const int _Type, const char* _Buf, const int _Length)
{
	ENetPacket *packet;
	int IDLength = strlen(_ID)+1;
	char* NewBuf = new char[_Length+IDLength+10];

	sprintf(NewBuf, "%c0000%s0000%s\0", 0, _ID, _Buf);

	memcpy((NewBuf+1), &IDLength, 4);
	memcpy((NewBuf+5+IDLength), &_Length, 4);

	packet = enet_packet_create(NewBuf, 10+IDLength+_Length, ENET_PACKET_FLAG_RELIABLE);

	if (packet == NULL)
	{
		Message("ENetServer", "Unable to create packet for sending.");
		return false;
	}
	enet_peer_send(&m_Server->peers[_Client], _Type, packet);

	Flush();
	
	delete []NewBuf;

	return true;
}

bool ENetServer::SendPacket(const int _Client, Network::Packet* _Packet)
{
	ENetPacket *packet;

	char* ID = _Packet->GetID();
	int IDLength = strlen(ID)+1;
	char* NewBuf = new char[5+IDLength+_Packet->GetLength()];
	char* ptr;

	sprintf(NewBuf, "%c0000%s", 1, ID);
	MemoryCopy(NewBuf+1, &IDLength, 4);
	ptr = (NewBuf+5+IDLength);

	MemoryCopy(ptr, _Packet->GetData(), _Packet->GetLength());

	packet = enet_packet_create(NewBuf, 5+IDLength+_Packet->GetLength(), ENET_PACKET_FLAG_RELIABLE);

	if (packet == NULL)
	{
		Message("ENetServer", "Unable to create packet for sending.");
		return false;
	}

	enet_peer_send(&m_Server->peers[_Client], _Packet->GetTag(), packet);

	Flush();

	delete []NewBuf;

	return true;
}

bool ENetServer::SendAll(const int _Type, const char* _Buf)
{
	return SendAllID("und", _Type, _Buf, strlen(_Buf));
}

bool ENetServer::SendAllL(const int _Type, const char* _Buf, const int _Length)
{
	return SendAllID("und", _Type, _Buf, _Length);
}

bool ENetServer::SendAllID(const char* _ID, const int _Type, const char* _Buf, const int _Length)
{
	ENetPacket *packet;
	ENetPeer *peer;
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
	enet_host_broadcast(m_Server, _Type, packet);

	Flush();
	
	delete []NewBuf;

	return true;
}

bool ENetServer::SendAllPacket(Network::Packet* _Packet)
{
	ENetPacket *packet;

	char* ID = _Packet->GetID();
	int IDLength = strlen(ID)+1;
	char* NewBuf = new char[5+IDLength+_Packet->GetLength()];
	char* ptr;

	sprintf(NewBuf, "%c0000%s", 1, ID);
	MemoryCopy(NewBuf+1, &IDLength, 4);
	ptr = (NewBuf+5+IDLength);

	MemoryCopy(ptr, _Packet->GetData(), _Packet->GetLength());

	packet = enet_packet_create(NewBuf, 5+IDLength+_Packet->GetLength(), ENET_PACKET_FLAG_RELIABLE);

	if (packet == NULL)
	{
		Message("ENetServer", "Unable to create packet for sending.");
		return false;
	}

	enet_host_broadcast(m_Server, _Packet->GetTag(), packet);

	Flush();
	delete []NewBuf;

	return true;
}

int ENetServer::NumClients()
{
	return m_Clients;
}

void ENetServer::Flush()
{
	// Force send the packet. Since *_flush doesn't work, we have to do it this way.
	Network::Packet *rpack = RecvNonBlocking(0);
	if (rpack != NULL)
	{
		m_BufferedPackets.push_back(rpack);
	}
}

