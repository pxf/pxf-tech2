#ifndef _PXF_MODULES_NET_ENETSERVER_H_
#define _PXF_MODULES_NET_ENETSERVER_H_

#include <Pxf/Network/Server.h>
#include <Pxf/Network/Packet.h>

#include <Pxf/Modules/net/ENetDevice.h>
#include <Pxf/Modules/net/ENetDefs.h>
#include <Pxf/Base/Platform.h>

#include <Pxf/Util/Array.h>
#include <enet/enet.h>
#include <stdio.h>
#include <string.h>

namespace Pxf
{
	namespace Network
	{
		class Server;
		class Packet;
	}

	namespace Modules
	{
		class ENetServer : public Pxf::Network::Server
		{
		private:
			ENetAddress m_Address;
			ENetHost *m_Server;
			Network::NetworkDevice* m_NetDev;
			
			int CreateClientID();

			Util::Array<Network::Packet*> m_BufferedPackets;

			void Flush();

			int m_Clients;

		public:
			ENetServer(Network::NetworkDevice* _NetworkDevice)
				: m_NetDev(_NetworkDevice)
				, m_Server(NULL)
				, m_Clients(0)
			{}
			virtual ~ENetServer(){};
			
			virtual bool Bind(const int _Port);
			virtual bool Shutdown();
			
			virtual Network::Packet* Recv();
			virtual Network::Packet* RecvNonBlocking(const int _Timeout);

			virtual int NumClients();

			// Single target.
			virtual bool Send(const int _Client, const int _Type, const char* _Buf);
			virtual bool SendID(const int _Client, const char* _ID, const int _Type, const char* _Buf, const int _Length);
			virtual bool SendPacket(const int _Client, Network::Packet* _Packet);

			// Broadcast targets.
			virtual bool SendAll(const int _Type, const char* _Buf);
			virtual bool SendAllL(const int _Type, const char* _Buf, const int _Length);
			virtual bool SendAllID(const char* _ID, const int _Type, const char* _Buf, const int _Length);
			virtual bool SendAllPacket(Network::Packet* _Packet);
		};
	}
}

#endif /* _PXF_MODULES_NET_ENETSERVER_H_ */
