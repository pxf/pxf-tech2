#ifndef _PXF_MODULES_NET_ENETCLIENT_H_
#define _PXF_MODULES_NET_ENETCLIENT_H_

#include <Pxf/Network/Client.h>
#include <Pxf/Network/Packet.h>

#include <Pxf/Modules/net/ENetDevice.h>
#include <Pxf/Modules/net/ENetDefs.h>

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
			ENetAddress Address;
			ENetHost *Client;
			ENetPeer *Peer;

		public:
			ENetClient(const char* _Host, const int _Port);

			virtual bool Connect();
			virtual bool Disconnect();
			virtual bool Connected();

			virtual Network::Packet* Recv();
			virtual bool Send(const int _Type, const char* _Buf);
		};
	}
}

#endif /* _PXF_MODULES_NET_ENETCLIENT_H_ */
