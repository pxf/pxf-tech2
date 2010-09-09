#ifndef _PXF_MODULES_NET_ENETSERVER_H_
#define _PXF_MODULES_NET_ENETSERVER_H_

#include <stdio.h>

#include <Pxf/Modules/net/ENetDevice.h>

#include <enet/enet.h>

namespace Pxf
{
	namespace Network
	{
		class Server;
	}

	namespace Modules
	{
		class ENetServer
		{
		private:
			ENetAddress Address;
			ENetHost *Server;

			/* Last packet */
			int LPLength
				, LPChannel
				, LPSource;
			char LPData[MAX_PACKET_SIZE+1];

		public:
			ENetServer(const int _Port);

			virtual bool Bind();
			virtual bool Shutdown();

			virtual int Recv(char* _Buf);
			virtual bool Send(const int _Client, const char* _Buf, const int _Length);
			virtual bool SendAll(const char* _Buf, const int _Length);
		};
	}
}

#endif /* _PXF_MODULES_NET_ENETSERVER_H_ */
