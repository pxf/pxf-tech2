#include <Pxf/Modules/net/ENetDataPacket.h>
#include <Pxf/Base/Memory.h>
#include <Pxf/Base/Debug.h>

#include <Pxf/Base/String.h>

using namespace Pxf::Modules;
using namespace Pxf;

// Empty package packet - multiple objects.
ENetDataPacket::ENetDataPacket(const char* _ID, const int _Sender, const int _Tag)
{
	m_Package = true;
	m_ReadOnly = false;
	m_Data = NULL;
	m_ID = new char[strlen(_ID)+1];
	MemoryCopy(m_ID, _ID, strlen(_ID)+1);
	m_PackageLength = 0;
	m_Sender = _Sender;
	m_Tag = _Tag;

	m_ObjectsBegin = 0;
}

// Filled normal/package packet - single object.
ENetDataPacket::ENetDataPacket(char* _Data, const int _Sender, const int _Length, const int _Tag)
{
	m_Package = false;
	m_ReadOnly = true;

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
	else if (*_Data == '\1')
	{ 
		m_Package = true;
		int IDLength;

		MemoryCopy(&IDLength, _Data, sizeof(IDLength));
		m_ID = new char[IDLength];
		MemoryCopy(m_ID, (_Data+sizeof(IDLength)), IDLength);

		m_Data = new char[_Length];
		MemoryCopy(m_Data, _Data, _Length);

		m_Package = true;
		m_PackageLength = _Length;
	}
	else
	{
		printf("UNDEFINED.\n");
/*		m_ID = new char[4];
		strcpy(m_ID, "und\0");
		m_Data = new char[_Length+1];
		strcpy(m_Data, _Data);
		m_Length = _Length;*/
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
	char* NewData = new char[m_PackageLength+sizeof(_Type)+sizeof(_Size)+_Size];
	MemoryCopy(NewData, m_Data, m_PackageLength);
	char* ptr = (NewData+m_PackageLength);
	m_PackageLength += _Size + sizeof(_Type) + sizeof(_Size);

	// _Type, Size, _Buffer
	MemoryCopy(ptr, &_Type, sizeof(_Type));
	ptr += sizeof(_Type);
	MemoryCopy(ptr, &_Size, sizeof(_Size));
	ptr += sizeof(_Size);
	MemoryCopy(ptr, _Buffer, _Size);

	if (m_Data != NULL)
		delete []m_Data;
	m_Data = NewData;

	return true;
}

bool ENetDataPacket::ReadObject(void* _Buffer, const int _Pos)
{
	char* ptr = (m_Data+m_ObjectsBegin);
	int Size, Type, Pos;

	for(Pos=0; Pos<=_Pos; Pos++)
	{
		// Out of bounds.
		if ((ptr-m_Data) >= m_PackageLength)
			return false;
			
		MemoryCopy(&Type, ptr, sizeof(Type));
		ptr += sizeof(Type);
		MemoryCopy(&Size, ptr, sizeof(Size));
		ptr += sizeof(Size);

		if (Pos == _Pos)
		{
			MemoryCopy(_Buffer, ptr, Size);
			return true;
		}
		else
			ptr += Size;
	}
	
	return true;
}

int ENetDataPacket::ObjectType(const int _Pos)
{
	char* ptr = (m_Data+m_ObjectsBegin);
	int Size, Type, Pos;

	for(Pos=0; Pos<=_Pos; Pos++)
	{
		// Out of bounds.
		if ((ptr-m_Data) >= m_PackageLength)
			return -1;
			
		MemoryCopy(&Type, ptr, sizeof(Type));
		ptr += sizeof(Type);
		MemoryCopy(&Size, ptr, sizeof(Size));
		ptr += sizeof(Size);
		ptr += Size;

		if (Pos == _Pos)
			return Type;
	}
	
	return 0;
}

