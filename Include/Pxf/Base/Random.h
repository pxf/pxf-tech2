#ifndef _PXF_BASE_RANDOM_H_
#define _PXF_BASE_RANDOM_H_

namespace Pxf {
    /* Random number generation */
    
    void RandSetSeed(unsigned long seed);
    unsigned long RandUI32();
    long RandI32();
    double RandFP64();
    float RandFP32();

}
#endif // _PXF_BASE_RANDOM_H_