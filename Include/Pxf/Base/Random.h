#ifndef _PXF_BASE_RANDOM_H_
#define _PXF_BASE_RANDOM_H_

/*
	Random number generation

	Using mersenne twister for good distribution.

	USE ME. rand() is crap.
 */

namespace Pxf {
	
	void RandSetSeed(unsigned long seed);
	unsigned long RandUI32();
	long RandI32();
	double RandFP64();
	float RandFP32();

}
#endif // _PXF_BASE_RANDOM_H_