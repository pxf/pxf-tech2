#ifndef _PXF_MODULES_SAMPLEMODULE_H_
#define _PXF_MODULES_SAMPLEMODULE_H_

#include <Pxf/Base/Debug.h>
#include <Pxf/Module.h>
#include <Pxf/Kernel.h>

namespace Pxf {
namespace Modules
{
    class SampleModule : public Pxf::Module
    {
        private:
        public:
        SampleModule(const char* _Identifier, unsigned _KernelVersion, unsigned _ApiVersion)
            : Module(_Identifier, _KernelVersion, _ApiVersion)
        {}
        
        virtual bool RegisterSystem(Pxf::Kernel* _Kernel, unsigned _SystemType);
    };
}}

#endif // _PXF_MODULES_SAMPLEMODULE_H_