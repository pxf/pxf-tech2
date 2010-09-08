#ifndef _PXF_NETWORK_CLIENT_H_
#define _PXF_NETWORK_CLIENT_H_

namespace Pxf
{
	namespace Network
	{
		class Client
		{
		public:
			virtual bool Connect() = 0;
			virtual bool Disconnect() = 0;
			virtual bool Connected() = 0;

			virtual int Recv(char* _Buf) = 0;
			virtual bool Send(const char* _Buf, const int _Length) = 0;
		};
	}
}

#endif /* _PXF_NETWORK_CLIENT_H_ */
