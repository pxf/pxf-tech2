#ifndef _PXF_NETWORK_PACKET_H_
#define _PXF_NETWORK_PACKET_H_

namespace Pxf
{
	namespace Network
	{
		class Packet
		{
		public:
			virtual ~Packet()
			{}

			virtual char* GetData() = 0;
			virtual int GetSender() = 0;
			virtual int GetLength() = 0;
		};
	}
}

#endif /* _PXF_NETWORK_PACKET_H_ */
