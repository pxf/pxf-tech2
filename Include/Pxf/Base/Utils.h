#ifndef _PXF_BASE_UTILS_H_
#define _PXF_BASE_UTILS_H_

#include <Pxf/Base/Types.h>
#include <Pxf/Base/Config.h>

#define PXF_PACKSHORT2(maj, min) ((((uint32)(maj)) << 16) | (min))

namespace Pxf
{

	// safe deletion of ptr
	template<typename T> 
	inline void SafeDelete(T& _Ptr)
	{
		if(_Ptr != 0) 
		{ 
			delete _Ptr;
			_Ptr = 0;
		}
	}

	// safe deletion of array
	template<typename T> 
	inline void SafeDeleteArray(T& _Ptr)
	{
		if(_Ptr != 0)
		{
			delete [] _Ptr;
			_Ptr = 0;
		}
	}

	// Swaps the content of a and b
	template <typename T> 
	inline void Swap(T& a, T& b)
	{
		T t; t = a; a = b; b = t;
	}

	/* Numeric compare functions */

	/* Returns the absolute value of a */
	template <typename T> 
	inline T Abs(const T& a)
	{
		return (a < 0 ? -a : a);
	}

	/* Returns the biggest of a and b */
	template<typename T1, typename T2> 
	inline T1 Max(const T1& a, const T2& b)
	{
		return (b < a ? a : b);
	}

	/* Returns the smallest of a and b */
	template<typename T1, typename T2> 
	inline T1 Min(const T1& a,const T2& b)
	{
		return (a < b ? a : b);
	}

	/* Clamps value between min and max */
	template <typename T1, typename T2, typename T3> 
	inline T1 Clamp(const T1& value, const T2& min, const T3& max)
	{
		return (value < min ? min : (value > max ? max : value));
	}

	// Reverse the bits of a byte
	inline uint8 BitReverse8(uint8 v)
	{
		return (uint8)(((v * 0x0802LU & 0x22110LU) | (v * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16);
	}

	inline uint32 PackShort2(uint16 _Maj, uint16 _Min)
	{
		return (((uint32)_Maj) << 16) | _Min;
	}
	
	inline void UnpackShort2(uint32 _Encoded, uint16* _pMaj, uint16* _pMin)
	{
		*_pMin = _Encoded & 0x7f;
		*_pMaj = (_Encoded >> 16) & 0x7f;
	}

	// Merge two bytes into a short
	inline uint16 Merge2(char a, char b)
	{
		return (a | (b << 8));
	}

	// Merge four bytes into a long
	inline uint32  Merge4(char a, char b, char c, char d)
	{
		return (a | (b << 8) | (c << 16) | (d << 24));
	}

	// Checks if an integer v is a power of 2
	inline bool IsPow2(uint32 v)
	{
		return (v & (v-1)) == 0;
	}

	// Checks if an integer v is even
	inline bool IsEven(uint32 v)
	{
		return !(v & 1);
	}

	// Swap two bytes
	inline uint16 Byteswap16(uint16 v)
	{
		return (v >> 8) | ((v & 0xFF) << 8);
	}

	// Swap four bytes
	inline uint32 Byteswap32(uint32 v)
	{
		return ((v>>24) | ((v&0xFF0000)>>8) | ((v&0xFF00)<<8) | ((v&0xFF)<<24));
	}

	template<typename T> 
	inline void SetBit(T* _value, unsigned _bit)
	{
		*_value |= (1 << _bit);
	}

	template<typename T> 
	inline void ClearBit(T* _value, unsigned _bit)
	{
		*_value &= ~(1 << _bit);
	}

	// Swaps endianess for arbitrary data type 
	template<typename T> 
	static T ByteswapArb(const T& t)
	{
		if (sizeof(T) > 1)
		{
			T ret;
			char* rptr = (char*)&t + sizeof(T) - 1;
			char* wptr = (char*)&ret;
			for(uint len = sizeof(T); len > 0; len--)
			{
				*wptr = *rptr;
				wptr++; rptr--;
			}
			return ret;
		}
		return t;
	}

	// Set up routines for swapping from/to "neutral", little and big endian
	#if defined (CONF_CPU_ENDIAN_LITTLE)
		template <typename T> inline T ByteswapBtoN(const T& t) { return ByteswapArb(t); }
		template <typename T> inline T ByteswapNtoB(const T& t) { return ByteswapArb(t); }
		template <typename T> inline T ByteswapLtoN(const T& t) { return t; }
		template <typename T> inline T ByteswapNtoL(const T& t) { return t; }
	#elif defined (CONF_CPU_ENDIAN_BIG)
		template <typename T> inline T ByteswapBtoN(const T& t) { return t; }
		template <typename T> inline T ByteswapNtoB(const T& t) { return t; }
		template <typename T> inline T ByteswapLtoN(const T& t) { return ByteswapArb(t); }
		template <typename T> inline T ByteswapNtoL(const T& t) { return ByteswapArb(t); }
	#else
		#error unable to generate bitswapping functions for your platform. endianess unknown.
	#endif
	
/* Usefull macros */
#define PXF_ELEMENTS_OF(t) (sizeof((t))/sizeof((t)[0]))
#define PXF_OFFSET_OF(s,m) (size_t)&reinterpret_cast<const volatile char&>((((s *)0)->m))
} // namespace Pxf

#endif //_PXF_BASE_UTILS_H_

