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

			virtual char* Data() = 0;
			virtual int Sender() = 0;
			virtual int Length() = 0;
		};
	}
}

#endif /* _PXF_NETWORK_PACKET_H_ */
