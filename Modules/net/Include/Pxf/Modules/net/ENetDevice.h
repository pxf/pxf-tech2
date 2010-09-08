#ifndef _PXF_MODULES_NET_ENETDEVICE_H_
#define _PXF_MODULES_NET_ENETDEVICE_H_

#include <Pxf/Network/NetworkDevice.h>
#include <Pxf/Network/Server.h>
#include <Pxf/Base/Debug.h>

#include <enet/enet.h>

namespace Pxf 
{
	namespace Network
	{
		class NetworkDevice;
		class Server;
		class Client;
	}

	namespace Modules
	{
		class ENetDevice : public Pxf::Network::NetworkDevice
		{
		private:
			virtual bool Init()
				{ return true; }

		public:
			ENetDevice(Pxf::Kernel* _Kernel);

			virtual Network::Server* CreateServer(const int _Port);
			virtual Network::Client* CreateClient(const char* _Host, const int _Port);

			virtual Network::Server* GetServer(const int _ServerIdent);
			virtual Network::Client* GetClient(const int _ClientIdent);

			virtual void KillServer(const int _ServerIdent);
			virtual void KillClient(const int _ClientIdent);
		};
	}
}

#endif /* _PXF_MODULES_NET_ENETDEVICE_H_ */
