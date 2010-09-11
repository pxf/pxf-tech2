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
			virtual ~Client(){}

			virtual bool Connect() = 0;
			virtual bool Disconnect() = 0;
			virtual bool Connected() = 0;

			virtual Packet* Recv() = 0;
			virtual bool Send(const char* _Buf, const int _Length) = 0;
		};
	}
}

#endif /* _PXF_NETWORK_CLIENT_H_ */
