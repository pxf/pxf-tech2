#ifndef _PXF_NETWORK_NULLNETWORKDEVICE_H_
#define _PXF_NETWORK_NULLNETWORKDEVICE_H_

#include <Pxf/System.h>

namespace Pxf 
{
namespace Network
{
	class NetworkDevice : Pxf::System
	{
	private:
		virtual bool Init()
		{ return true; }

	public:
		NullNetworkDevice(Kernel* _Kernel)
			: Pxf::Network::NetworkDevice(_Kernel, "Null Network Device")
		{}

		virtual bool Connected() 
		{ return false; }
		virtual bool Recv(char* _Buf)
		{ return false; }
		virtual bool Send(const char* _Buf, const int _Length)
		{ return false; }

		/* server */
		virtual bool Bind(const char* _Host, const int _Port)
		{ return false; }

		/* client */
		virtual bool Connect(const char* _Host, const int _Port)
		{ return false; }
		virtual bool Disconnect()
		{ return false; }
	};
}
}

#endif /* _PXF_NETWORK_NULLNETWORKDEVICE_H_ */
