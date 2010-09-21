#ifndef _PXF_NETWORK_ENETPACKET_H_
#define _PXF_NETWORK_ENETPACKET_H_

#include <Pxf/Network/Packet.h>

#include <Pxf/Modules/net/ENetDevice.h>

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
			int Tag;

		public:
			ENetDataPacket(char* _Data, const int _Sender, const int _Length, const int _Tag);
			virtual ~ENetDataPacket();

			virtual char* GetData();
			virtual int GetSender();
			virtual int GetLength();
			virtual int GetTag();
		};
	}
}

#endif /* _PXF_NETWORK_PACKET_H_ */
