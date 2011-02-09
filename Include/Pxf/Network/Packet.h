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

			virtual char* GetData() = 0;
			virtual int GetSender() = 0;
			virtual int GetLength() = 0;
			virtual int GetTag() = 0;
			virtual char* GetID() = 0;

			virtual bool PushObject(const int _Type, const void* _Buffer, unsigned int _Size) = 0;
			virtual bool ReadObject(void* _Buffer, const int _BufferSize, const int _Pos) = 0;

			/* TYPES:
			 * 0 = int
			 * 1 = float
			 * 2 = char*
			 */ 
			virtual bool PushInt(const int _Int)
			{
				return PushObject(0, &_Int, sizeof(int));
			}

			virtual bool PushFloat(const float _Float)
			{
				return PushObject(1, &_Float, sizeof(float));
			}

			virtual bool PushString(const char* _Buffer, unsigned int _Size)
			{
				return PushObject(2, _Buffer, _Size+1);
			}

			template<typename T> T GetArray(const int _Size, const int _Pos)
			{
				char* v = new char[_Size];

				ReadObject(v, _Size, _Pos);

				return v;
			}

			template<typename T> T GetObject(const int _Pos) 
			{ 
				T v;
				ReadObject(&v, ObjectSize(_Pos), _Pos);
					
				return v;
			}
			virtual int ObjectType(const int _Pos) = 0;
			virtual int ObjectSize(const int _Pos) = 0;
		};
	}
}

#endif /* _PXF_NETWORK_PACKET_H_ */
