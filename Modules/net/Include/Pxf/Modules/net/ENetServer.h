#ifndef _PXF_MODULES_NET_ENETSERVER_H_
#define _PXF_MODULES_NET_ENETSERVER_H_

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
		public:
			ENetServer();

			virtual bool Bind() = 0;
			virtual bool Shutdown() = 0;

			virtual int Recv(char* _Buf) = 0;
			virtual bool Send(const int _Client, const char* _Buf, const int _Length) = 0;
			virtual bool SendAll(const char* _Buf, const int _Length) = 0;
		};
	}
}

#endif /* _PXF_MODULES_NET_ENETSERVER_H_ */
