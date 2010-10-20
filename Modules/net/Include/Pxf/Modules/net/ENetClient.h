#ifndef _PXF_MODULES_NET_ENETCLIENT_H_
#define _PXF_MODULES_NET_ENETCLIENT_H_

#include <Pxf/Network/Client.h>
#include <Pxf/Network/Packet.h>

#include <Pxf/Modules/net/ENetDevice.h>
#include <Pxf/Modules/net/ENetDefs.h>
#include <Pxf/Base/Platform.h>

#include <Pxf/Util/Array.h>
#include <Pxf/Base/Memory.h>

#include <enet/enet.h>
#include <string.h>

namespace Pxf 
{
	namespace Network
	{
		class Client;
		class Packet;
	}

	namespace Modules
	{
		class ENetClient : public Pxf::Network::Client
		{
		private:
			ENetAddress m_Address;
			ENetHost *m_Client;
			ENetPeer *m_Peer;
			Network::NetworkDevice* m_NetDev;

			bool m_Connected;
			
			Util::Array<Network::Packet*> m_BufferedPackets;

			void Flush();
			
		public:
			//ENetClient(const char* _Host, const int _Port);
			ENetClient(Network::NetworkDevice* _NetworkDevice)
				: m_NetDev(_NetworkDevice)
				, m_Connected(false)
				, m_Client(NULL)
				, m_Peer(NULL)
			{}

			virtual bool Connect(const char* _Host, const int _Port);
			virtual bool Disconnect();
			virtual bool Connected();

			virtual Network::Packet* Recv();
			virtual Network::Packet* RecvNonBlocking(const int _Timeout);
			virtual bool Send(const int _Type, const char* _Buf, const int _Length);
			virtual bool SendID(const char* _ID, const int _Type, const char* _Buf, const int _Length);
			virtual bool SendPacket(Network::Packet* _Packet);
		};
	}
}

#endif /* _PXF_MODULES_NET_ENETCLIENT_H_ */
