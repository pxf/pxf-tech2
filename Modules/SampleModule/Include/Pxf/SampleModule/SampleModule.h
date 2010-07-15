#ifndef _PXF_SAMPLEMODULE_SAMPLEMODULE_H_
#define _PXF_SAMPLEMODULE_SAMPLEMODULE_H_

#include <Pxf/Base/Module.h>

class SampleModule : public Pxf::Base::Module
{
    private:
    public:
    SampleModule(unsigned ApiVersion)
        : Module(ApiVersion)
    {}
};

#endif // _PXF_SAMPLEMODULE_SAMPLEMODULE_H_