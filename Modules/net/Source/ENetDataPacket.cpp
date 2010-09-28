#include <Pxf/Modules/net/ENetDataPacket.h>
#include <Pxf/Base/Debug.h>

#include <string.h>

using namespace Pxf::Modules;

ENetDataPacket::ENetDataPacket(char* _Data, const int _Sender, const int _Length, const int _Tag)
{
/*	if (*_Data == 10 && *(_Data+1) == 20)
	{
		// It's an ID hidden in the beginning.
		ID = *(_Data+2);
		Data = new char[_Length-2];
		strcpy(Data, _Data+2);
	}
	else
	{*/
		Data = new char[_Length+1];
		strcpy(Data, _Data);
//	}
	Sender = _Sender;
	Length = _Length;
	Tag = _Tag;
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

int ENetDataPacket::GetTag()
{
	return Tag;
}
