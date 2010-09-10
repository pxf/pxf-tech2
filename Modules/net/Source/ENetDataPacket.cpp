#include <Pxf/Modules/net/ENetDataPacket.h>

using namespace Pxf::Modules;

ENetDataPacket::ENetDataPacket(char* _Data, const int _Sender, const int _Length)
{
	Data = _Data;
	Sender = _Sender;
	Length = _Length;
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