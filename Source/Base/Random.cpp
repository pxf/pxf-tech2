#include <Pxf/Base/Random.h>

/*
 * Random number generation, Mersenne twister
 *
 * http://en.literateprograms.org/Mersenne_twister_(C)
 *****************************************************************************/

#define PXF__MT_LEN	   624
#define PXF__MT_IA		397
#define PXF__MT_IB		(PXF__MT_LEN - PXF__MT_IA)
#define PXF__UPPER_MASK   0x80000000
#define PXF__LOWER_MASK   0x7FFFFFFF
#define PXF__MATRIX_A	 0x9908B0DF
#define PXF__TWIST(b,i,j) ((b)[i] & PXF__UPPER_MASK) | ((b)[j] & PXF__LOWER_MASK)
#define PXF__MAGIC(s)	 (((s)&1)*PXF__MATRIX_A)


int mt_index = 0;
unsigned long mt_buffer[PXF__MT_LEN];

void Pxf::RandSetSeed(unsigned long seed)
{
	int i;
	mt_buffer[0] = seed & 0xffffffffUL;
	for (i = 1; i < PXF__MT_LEN; i++)
		mt_buffer[i] = (1812433253UL * (mt_buffer[i-1] ^ (mt_buffer[i-1] >> 30)) + i) & 0xffffffffUL;
}

// Interval: [0, 0xffffffff]
unsigned long Pxf::RandUI32()
{
	unsigned long * b = mt_buffer;
	int idx = mt_index;
	unsigned long s;
	int i;

	if (idx == PXF__MT_LEN*sizeof(unsigned long))
	{
		idx = 0;
		i = 0;
		for (; i < PXF__MT_IB; i++) {
			s = PXF__TWIST(b, i, i+1);
			b[i] = b[i + PXF__MT_IA] ^ (s >> 1) ^ PXF__MAGIC(s);
		}
		for (; i < PXF__MT_LEN-1; i++) {
			s = PXF__TWIST(b, i, i+1);
			b[i] = b[i - PXF__MT_IB] ^ (s >> 1) ^ PXF__MAGIC(s);
		}
		
		s = PXF__TWIST(b, PXF__MT_LEN-1, 0);
		b[PXF__MT_LEN-1] = b[PXF__MT_IA-1] ^ (s >> 1) ^ PXF__MAGIC(s);
	}
	mt_index = idx + sizeof(unsigned long);
	return *(unsigned long *)((unsigned char *)b + idx);
}

// Interval: [0, 0x7fffffff]
long  Pxf::RandI32()
{
	return (long)(RandUI32() >> 1);
}

// Interval: [0, 1]
double  Pxf::RandFP64()
{
	return RandUI32() / ((double) (1 << 16) * (1 << 16));
}

float  Pxf::RandFP32()
{
	return RandUI32() / ((float) (1 << 16) * (1 << 16));
}