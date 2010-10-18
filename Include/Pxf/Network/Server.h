#ifndef _PXF_NETWORK_SERVER_H_
#define _PXF_NETWORK_SERVER_H_

namespace Pxf
{
	namespace Network
	{
		class Packet;

		class Server
		{
		public:
			int Ident;

			virtual ~Server(){}

			virtual bool Bind(const int _Port) = 0;
			virtual bool Shutdown() = 0;

			virtual Packet* Recv() = 0;
			virtual Packet* RecvNonBlocking(const int _Timeout) = 0;
			virtual bool Send(const int _Client, const int _Type, const char* _Buf) = 0;
			virtual bool SendAll(const int _Type, const char* _Buf) = 0;
			virtual bool SendAllL(const int _Type, const char* _Buf, const int _Length) = 0;
			virtual bool SendAllID(const char* _ID, const int _Type, const char* _Buf, const int _Length) = 0;
			virtual bool SendAllPacket(Packet* _Packet) = 0;
		};
	}
}

#endif /* _PXF_NETWORK_SERVER_H_ */
