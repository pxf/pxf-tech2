#ifndef _PXF_NETWORK_ENETDATAPACKET_H_
#define _PXF_NETWORK_ENETDATAPACKET_H_

#include <Pxf/Network/Packet.h>
#include <Pxf/Network/NetworkDefs.h>

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
			char* ID;

			bool m_Package;
			int m_PackageLength;

		public:
			ENetDataPacket(const int _Sender, const int _Tag);
			ENetDataPacket(char* _Data, const int _Sender, const int _Length, const int _Tag);
			virtual ~ENetDataPacket();

			virtual char* GetData();
			virtual int GetSender();
			virtual int GetLength();
			virtual int GetTag();
			virtual char* GetID();

			virtual bool PushObject(const int _Type, const void* _Buffer, unsigned int _Size);
			virtual bool ReadObject(void* _Buffer, const int _Pos);
		};
	}
}

#endif /* _PXF_NETWORK_ENETDATAPACKET_H_ */
