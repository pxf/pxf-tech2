#ifndef _PXF_MODULES_NET_ENETCLIENT_H_
#define _PXF_MODULES_NET_ENETCLIENT_H_

#include <Pxf/Network/Client.h>
#include <Pxf/Network/Packet.h>

#include <Pxf/Modules/net/ENetDevice.h>
#include <Pxf/Modules/net/ENetDefs.h>
#include <Pxf/Base/Platform.h>

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
			int Ident;
			
			//ENetClient(const char* _Host, const int _Port);

			virtual bool Connect(const char* _Host, const int _Port);
			virtual bool Disconnect();
			virtual bool Connected();

			virtual Network::Packet* Recv();
			virtual Network::Packet* RecvNonBlocking(const int _Timeout);
			virtual bool Send(const int _Type, const char* _Buf);

		};
	}
}

#endif /* _PXF_MODULES_NET_ENETCLIENT_H_ */
