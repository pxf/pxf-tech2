#ifndef _PXF_MODULES_NET_ENETCLIENT_H_
#define _PXF_MODULES_NET_ENETCLIENT_H_

#include <Pxf/Network/NetworkDevice.h>
#include <Pxf/Base/Debug.h>

#include <enet/enet.h>

namespace Pxf 
{
	namespace Network
	{
//		class NetworkDevice;
		class Client;
	}

	namespace Modules
	{
		class ENetClient
		{
		public:
			ENetClient();

			virtual bool Connect() = 0;
			virtual bool Disconnect() = 0;
			virtual bool Connected() = 0;

			virtual int Recv(char* _Buf) = 0;
			virtual bool Send(const char* _Buf, const int _Length) = 0;
		};
	}
}

#endif /* _PXF_MODULES_NET_ENETCLIENT_H_ */
