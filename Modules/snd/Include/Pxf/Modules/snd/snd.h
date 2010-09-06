#ifndef _PXF_MODULES_SND_H_
#define _PXF_MODULES_SND_H_

#include <Pxf/Base/Debug.h>
#include <Pxf/Module.h>
#include <Pxf/Kernel.h>

namespace Pxf {
namespace Modules
{
    class AudioImporter : public Pxf::Module
    {
        private:
        public:
        AudioImporter(const char* _Identifier, unsigned _KernelVersion, unsigned _ApiVersion)
            : Module(_Identifier, _KernelVersion, _ApiVersion)
        {}
        
        virtual bool RegisterSystem(Pxf::Kernel* _Kernel, unsigned _SystemType);
    };
    
}}

#endif // _PXF_MODULES_SND_H_