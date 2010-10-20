#ifndef _PXF_MODULES_NET_ENETDEVICE_H_
#define _PXF_MODULES_NET_ENETDEVICE_H_

#include <Pxf/Network/NetworkDevice.h>
#include <Pxf/Network/Server.h>
#include <Pxf/Network/Client.h>

#include <Pxf/Modules/net/ENetServer.h>
#include <Pxf/Modules/net/ENetClient.h>
#include <Pxf/Modules/net/ENetDefs.h>
#include <Pxf/Modules/net/ENetDataPacket.h>

#include <Pxf/Util/Array.h>
#include <Pxf/Base/Debug.h>
#include <enet/enet.h>

namespace Pxf 
{
	namespace Network
	{
		class NetworkDevice;
		class Server;
		class Client;
		class Packet;
	}

	namespace Modules
	{
		class ENetClient;
		class ENetServer;

		class ENetDevice : public Pxf::Network::NetworkDevice
		{
		private:
			virtual bool Init()
				{ return true; }

			Util::Array<ENetServer*> Servers;
			Util::Array<ENetClient*> Clients;

			Util::Array<char*> Tags;

			int ChildID;

		public:
			ENetDevice(Pxf::Kernel* _Kernel);
			~ENetDevice();

			virtual Network::Server* CreateServer();
			virtual Network::Client* CreateClient();

			virtual Network::Server* GetServer(const int _ServerIdent);
			virtual Network::Client* GetClient(const int _ClientIdent);

			virtual void KillServer(const int _ServerIdent);
			virtual void KillClient(const int _ClientIdent);
			
			virtual int AddTag(char* _Name);
			virtual Util::Array<char*>* GetTags();

			virtual Network::Packet* CreateEmptyPacket(const char* _ID, const int _Tag);
			virtual Network::Packet* CreatePacket(char* _Data, const int _Tag, const int _Length, const int _Sender);

			virtual int NumClients();
		};
	}
}

#endif /* _PXF_MODULES_NET_ENETDEVICE_H_ */
