#ifndef _PXF_NETWORK_ENETPACKET_H_
#define _PXF_NETWORK_ENETPACKET_H_

#include <Pxf/Network/Packet.h>

namespace Pxf
{
	namespace Network
	{
		class Packet;
	}

	namespace Modules
	{
		class ENetDataPacket : public Pxf::Network::Packet
		{
		private:
			char* Data;
			int Sender;
			int Length;

		public:
			ENetDataPacket(char* _Data, const int _Sender, const int _Length);
			virtual ~ENetDataPacket(){}

			char* GetData();
			int GetSender();
			int GetLength();
		};
	}
}

#endif /* _PXF_NETWORK_PACKET_H_ */
