#ifndef _PXF_NETWORK_NETWORKDEVICE_H_
#define _PXF_NETWORK_NETWORKDEVICE_H_

#include <Pxf/System.h>

namespace Pxf 
{
namespace Network
{
	class NetworkDevice : public Pxf::System
	{
	public:
		NetworkDevice(Kernel* _Kernel, const char* _Identifier)
			: Pxf::System(_Kernel, Pxf::System::SYSTEM_TYPE_NETWORKDEVICE, _Identifier)
		{}

		virtual bool Connected() = 0;
		virtual bool Recv(char* _Buf) = 0;
		virtual bool Send(const char* _Buf, const int _Length) = 0;

		/* server */
		virtual bool Bind(const int _Port) = 0;

		/* client */
		virtual bool Connect(const char* _Host, const int _Port) = 0;
		virtual bool Disconnect() = 0;
	};
}
}

#endif /* _PXF_NETWORK_NETWORKDEVICE_H_ */
