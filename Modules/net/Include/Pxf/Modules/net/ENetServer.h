#ifndef _PXF_NETWORK_NETWORKDEVICE_H_
#define _PXF_NETWORK_NETWORKDEVICE_H_

#include <Pxf/Network/NetworkDevice.h>
#include <Pxf/Base/Debug.h>

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
			virtual bool Init();
		public:
			ENetServer(Pxf::Kernel* _Kernel);

			virtual bool Connected();
			virtual bool Recv(char* _Buf);
			virtual bool Send(const char* _Buf, const int _Length);

			/* server */
			virtual bool Bind(const char* _Host, const int _Port);

			/* client */
			virtual bool Connect(const char* _Host, const int _Port);
			virtual bool Disconnect();
		};
	}
}

#endif /* _PXF_NETWORK_NETWORKDEVICE_H_ */
