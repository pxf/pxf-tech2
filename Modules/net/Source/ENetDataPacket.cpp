#include <Pxf/Modules/net/ENetDataPacket.h>
#include <Pxf/Base/Memory.h>
#include <Pxf/Base/Debug.h>

#include <Pxf/Base/String.h>

using namespace Pxf::Modules;
using namespace Pxf;

ENetDataPacket::ENetDataPacket(char* _Data, const int _Sender, const int _Length, const int _Tag)
{
	printf("Packet creation. length: %d\n", _Length);
	if (*_Data == '\0')
	{
		// It's an ID hidden in the beginning.
		
		int IDLength, DataLength;

		memcpy(&IDLength, (_Data+1), 4);
		memcpy(&DataLength, (_Data+1+IDLength+4), 4);

		//printf("idl: %d   ml: %d\n", IDLength, DataLength);

		ID = new char[IDLength+1];
		Data = new char[DataLength+1];
		MemoryCopy(ID, (_Data+5), IDLength);
		MemoryCopy(Data, (_Data+5+IDLength+4), DataLength);
		ID[IDLength] = '\0';
		Data[DataLength] = '\0';

		//printf("id: %s\n", ID);
		//printf("data: %s\n", Data);

		Length = DataLength;
	}
	else
	{
		ID = new char[4];
		strcpy(ID, "und\0");
		Data = new char[_Length+1];
		strcpy(Data, _Data);
		Length = _Length;
	}

	Sender = _Sender;
	Tag = _Tag;
}

ENetDataPacket::~ENetDataPacket()
{
	delete Data;
	delete ID;
}

char* ENetDataPacket::GetData()
{
	return Data;
}

int ENetDataPacket::GetSender()
{
	return Sender;
}

int ENetDataPacket::GetLength()
{
	return Length;
}

int ENetDataPacket::GetTag()
{
	return Tag;
}

char* ENetDataPacket::GetID()
{
	return ID;
}

bool ENetDataPacket::PushObject(const int _Type, const void* _Buffer, unsigned int _Size)
{
	return true;
}

bool ENetDataPacket::ReadObject(void* _Buffer, const int _Pos)
{
	return true;
}