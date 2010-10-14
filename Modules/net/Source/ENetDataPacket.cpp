#include <Pxf/Modules/net/ENetDataPacket.h>
#include <Pxf/Base/Memory.h>
#include <Pxf/Base/Debug.h>

#include <Pxf/Base/String.h>

using namespace Pxf::Modules;
using namespace Pxf;

// Filled package packet - multiple objects.
ENetDataPacket::ENetDataPacket(const char* _Data)
{
	// TODO:
}

// Empty package packet - multiple objects.
ENetDataPacket::ENetDataPacket(const int _Sender, const int _Tag)
{
	m_Package = true;
	m_Data = NULL;
	m_PackageLength = 0;
	m_Sender = _Sender;
	m_Tag = _Tag;
}

// Filled normal packet - single object.
ENetDataPacket::ENetDataPacket(char* _Data, const int _Sender, const int _Length, const int _Tag)
{
	m_Package = false;

	//printf("Packet creation. length: %d\n", _Length);
	if (*_Data == '\0')
	{
		// It's an ID hidden in the beginning.
		
		int IDLength, DataLength;

		MemoryCopy(&IDLength, (_Data+1), 4);
		MemoryCopy(&DataLength, (_Data+1+IDLength+4), 4);

		//printf("idl: %d   ml: %d\n", IDLength, DataLength);

		m_ID = new char[IDLength+1];
		m_Data = new char[DataLength+1];
		MemoryCopy(m_ID, (_Data+5), IDLength);
		MemoryCopy(m_Data, (_Data+5+IDLength+4), DataLength);
		m_ID[IDLength] = '\0';
		m_Data[DataLength] = '\0';

		//printf("id: %s\n", ID);
		//printf("data: %s\n", Data);

		m_Length = DataLength;
	}
	else
	{
		m_ID = new char[4];
		strcpy(m_ID, "und\0");
		m_Data = new char[_Length+1];
		strcpy(m_Data, _Data);
		m_Length = _Length;
	}

	m_Sender = _Sender;
	m_Tag = _Tag;
}

ENetDataPacket::~ENetDataPacket()
{
	delete []m_Data;
	delete []m_ID;
}

char* ENetDataPacket::GetData()
{
	return m_Data;
}

int ENetDataPacket::GetSender()
{
	return m_Sender;
}

int ENetDataPacket::GetLength()
{
	if (m_Package)
		return m_Length;
	else
		return m_PackageLength;
}

int ENetDataPacket::GetTag()
{
	return m_Tag;
}

char* ENetDataPacket::GetID()
{
	return m_ID;
}

bool ENetDataPacket::PushObject(const int _Type, const void* _Buffer, unsigned int _Size)
{
	m_PackageLength += _Size + sizeof(_Type) + sizeof(_Size);
	char* NewData = new char[m_PackageLength];
	char* ptr = (NewData+m_PackageLength);

	// _Type, Size, _Buffer
	MemoryCopy(ptr, &_Type, sizeof(_Type));
	ptr += sizeof(_Type);
	MemoryCopy(ptr, &_Size, sizeof(_Size));
	ptr += sizeof(_Size);
	MemoryCopy(ptr, _Buffer, _Size);

	delete []m_Data;
	m_Data = NewData;

	return true;
}

bool ENetDataPacket::ReadObject(void* _Buffer, const int _Pos)
{
	return true;
}
