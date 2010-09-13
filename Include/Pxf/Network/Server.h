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
			virtual ~Server(){}

			virtual bool Bind() = 0;
			virtual bool Shutdown() = 0;

			virtual Packet* Recv() = 0;
			virtual bool Send(const int _Client, const char* _Buf, const int _Length) = 0;
			virtual bool SendAll(const char* _Buf, const int _Length) = 0;
		};
	}
}

#endif /* _PXF_NETWORK_SERVER_H_ */
