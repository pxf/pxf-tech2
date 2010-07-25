#ifndef _PXF_MODULES_SAMPLEMODULE_H_
#define _PXF_MODULES_SAMPLEMODULE_H_

#include <Pxf/Base/Module.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Kernel.h>

namespace Pxf {
namespace Modules
{
    class SampleModule : public Pxf::Base::Module
    {
        private:
        public:
        SampleModule(const char* _Identifier, unsigned _KernelVersion, unsigned _ApiVersion)
            : Module(_Identifier, _KernelVersion, _ApiVersion)
        {}
        
        virtual void RegisterSystems(Pxf::Kernel* _Kernel);
    };
}}

#endif // _PXF_MODULES_SAMPLEMODULE_H_