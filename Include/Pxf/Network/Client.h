#ifndef _PXF_NETWORK_CLIENT_H_
#define _PXF_NETWORK_CLIENT_H_

namespace Pxf
{
	namespace Network
	{
		class Packet;

		class Client
		{
		public:
			int Ident;

			virtual ~Client(){}

			virtual bool Connect(const char* _Host, const int _Port) = 0;
			virtual bool Disconnect() = 0;
			virtual bool Connected() = 0;

			virtual Packet* Recv() = 0;
			virtual Packet* RecvNonBlocking(const int _Timeout) = 0;
			virtual bool Send(const int _Type, const char* _Buf) = 0;

		};
	}
}

#endif /* _PXF_NETWORK_CLIENT_H_ */
