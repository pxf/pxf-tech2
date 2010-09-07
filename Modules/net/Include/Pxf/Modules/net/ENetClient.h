#ifndef _PXF_MODULES_NET_ENETCLIENT_H_
#define _PXF_MODULES_NET_ENETCLIENT_H_

#include <Pxf/Network/NetworkDevice.h>
#include <Pxf/Base/Debug.h>

#include <enet/enet.h>

namespace Pxf 
{
	namespace Network
	{
		class NetworkDevice;
	}

	namespace Modules
	{
		class ENetClient : public Pxf::Network::NetworkDevice
		{
		private:
			virtual bool Init()
				{ return true; }

			ENetHost *Client;
			ENetPeer *Peer;
			ENetAddress Address;
		public:
			ENetClient(Pxf::Kernel* _Kernel);

			virtual bool Connected();
			virtual bool Recv(char* _Buf);
			virtual bool Send(const char* _Buf, const int _Length);

			/* server */
			virtual bool Bind(const int _Port)
				{ return false; }

			/* client */
			virtual bool Connect(const char* _Host, const int _Port);
			virtual bool Disconnect();
		};
	}
}

#endif /* _PXF_MODULES_NET_ENETCLIENT_H_ */
