#ifndef _PXF_SAMPLEMODULE_SAMPLEMODULE_H_
#define _PXF_SAMPLEMODULE_SAMPLEMODULE_H_

#include <Pxf/Base/Module.h>
#include <Pxf/Kernel.h>

#ifdef CONF_FAMILY_WINDOWS
    #ifdef CONF_MODULAR
        #define PXFMODULE extern "C" __declspec(dllexport)
    #else
        #define PXFMODULE
    #endif
#else
    #ifdef CONF_MODULAR
        #define PXFMODULE extern "C"
    #else
        #define PXFMODULE
    #endif
#endif

namespace Pxf {
namespace Modules
{
    class SampleModule : public Pxf::Base::Module
    {
        private:
        public:
        SampleModule(unsigned ApiVersion)
            : Module(ApiVersion)
        {}
        
        virtual void RegisterSystems(Pxf::Kernel* _Kernel)
        {
            
        }
    };
}}

#endif // _PXF_SAMPLEMODULE_SAMPLEMODULE_H_