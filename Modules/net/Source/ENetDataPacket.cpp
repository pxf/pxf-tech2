#include <Pxf/Modules/net/ENetDataPacket.h>
#include <Pxf/Base/Debug.h>

#include <string.h>

using namespace Pxf::Modules;

ENetDataPacket::ENetDataPacket(char* _Data, const int _Sender, const int _Length)
{
	Data = new char[_Length+1];
	strcpy(Data, _Data);
	Sender = _Sender;
	Length = _Length;
}

ENetDataPacket::~ENetDataPacket()
{
	delete Data;
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