#ifndef _PXF_NETWORK_NETWORKDEVICE_H_
#define _PXF_NETWORK_NETWORKDEVICE_H_

#include <Pxf/System.h>

#include <Pxf/Network/Server.h>
#include <Pxf/Network/Client.h>
#include <Pxf/Network/Packet.h>

namespace Pxf 
{
namespace Network
{
	class Server;
	class Client;

	class NetworkDevice : public Pxf::System
	{
	public:
		NetworkDevice(Kernel* _Kernel, const char* _Identifier)
			: Pxf::System(_Kernel, Pxf::System::SYSTEM_TYPE_NETWORKDEVICE, _Identifier)
		{}
		virtual ~NetworkDevice(){}

		virtual Server* CreateServer() = 0;
		virtual Client* CreateClient() = 0;

		virtual Server* GetServer(const int _ServerIdent) = 0;
		virtual Client* GetClient(const int _ClientIdent) = 0;

		virtual void KillServer(const int _ServerIdent) = 0;
		virtual void KillClient(const int _ClientIdent) = 0;
		
		virtual int CreateType() = 0;
	};
}
}

#endif /* _PXF_NETWORK_NETWORKDEVICE_H_ */
