#ifndef _PXF_MODULES_NET_ENETSERVER_H_
#define _PXF_MODULES_NET_ENETSERVER_H_

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
		class ENetServer : public Pxf::Network::NetworkDevice
		{
		private:
			virtual bool Init()
				{ return false; }

			ENetAddress Address;
			ENetHost *Server;

		public:
			ENetServer(Pxf::Kernel* _Kernel);

			virtual bool Connected()
				{ return false; }
			virtual bool Recv(char* _Buf);
			virtual bool Send(const char* _Buf, const int _Length);

			/* server */
			virtual bool Bind(const int _Port);

			/* client */
			virtual bool Connect(const char* _Host, const int _Port)
				{ return false; }
			virtual bool Disconnect()
				{ return false; }
		};
	}
}

#endif /* _PXF_MODULES_NET_ENETSERVER_H_ */
