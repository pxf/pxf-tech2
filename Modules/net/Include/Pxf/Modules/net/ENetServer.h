#ifndef _PXF_MODULES_NET_ENETSERVER_H_
#define _PXF_MODULES_NET_ENETSERVER_H_

#include <Pxf/Network/Server.h>
#include <Pxf/Network/Packet.h>

#include <Pxf/Modules/net/ENetDevice.h>
#include <Pxf/Modules/net/ENetDefs.h>

#include <Pxf/Util/Array.h>
#include <enet/enet.h>
#include <stdio.h>

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
			ENetAddress Address;
			ENetHost *Server;

			/* Last packet */
			int LPLength
				, LPChannel
				, LPSource;
			char LPData[MAX_PACKET_SIZE+1];

			Util::Array<ENetPeer*> Clients;

		public:
			ENetServer(const int _Port);

			virtual bool Bind();
			virtual bool Shutdown();

			virtual Network::Packet* Recv();
			virtual bool Send(const int _Client, const char* _Buf, const int _Length);
			virtual bool SendAll(const char* _Buf, const int _Length);
		};
	}
}

#endif /* _PXF_MODULES_NET_ENETSERVER_H_ */
