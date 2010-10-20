#include <Pxf/Base/Hash.h>
#include <Pxf/Base/Stream.h>

/******************************************************************************
* Hash functions
* - Super fast hash, from http://www.azillionmonkeys.com/qed/hash.html
*******************************************************************************/
#undef pxf_get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define pxf_get16bits(d) (*((const unsigned short *) (d)))
#endif

#if !defined (pxf_get16bits)
#define pxf_get16bits(d) ((((unsigned long)(((const unsigned char *)(d))[1])) << 8)+(unsigned long)(((const unsigned char *)(d))[0]) )
#endif

unsigned long Pxf::Hash(const char *data, int len)
{
	unsigned long hash = len, tmp;
	int rem;

	if (len <= 0 || data == 0) return 0;

	rem = len & 3;
	len >>= 2;

	/* Main loop */
	for (;len > 0; len--) {
		hash  += pxf_get16bits (data);
		tmp    = (pxf_get16bits (data+2) << 11) ^ hash;
		hash   = (hash << 16) ^ tmp;
		data  += 2*sizeof (unsigned short);
		hash  += hash >> 11;
	}

	/* Handle end cases */
	switch (rem) {
		case 3: hash += pxf_get16bits (data);
				hash ^= hash << 16;
				hash ^= data[sizeof (unsigned short)] << 18;
				hash += hash >> 11;
				break;
		case 2: hash += pxf_get16bits (data);
				hash ^= hash << 11;
				hash += hash >> 17;
				break;
		case 1: hash += *data;
				hash ^= hash << 10;
				hash += hash >> 1;
	}

	/* Force "avalanching" of final 127 bits */
	hash ^= hash << 3;
	hash += hash >> 5;
	hash ^= hash << 4;
	hash += hash >> 17;
	hash ^= hash << 25;
	hash += hash >> 6;

	return hash;
}

unsigned long Pxf::HashFile(const char* _FileName)
{
	FileStream s;
	if (!s.OpenReadBinary(_FileName))
		return 0;
	unsigned size = s.GetSize();
	char* data = new char[size];
	s.Read(data, size);
	unsigned long hash = Hash(data, size);
	s.Close();
	return hash;
}

unsigned long Pxf::HashPtr(void *ptr)
{
	unsigned long x = (unsigned long)ptr;
	x = x + (x>>6) + (x >> 19);
	x += x << 16;

	x ^= x << 3;
	x += x >> 5;
	x ^= x << 2;
	x += x >> 15;
	x ^= x << 10;
	return x + (x>>6) + (x >> 19);
}