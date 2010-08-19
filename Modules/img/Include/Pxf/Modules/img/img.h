#ifndef _PXF_MODULES_IMG_H_
#define _PXF_MODULES_IMG_H_

#include <Pxf/Base/Debug.h>
#include <Pxf/Module.h>
#include <Pxf/Kernel.h>

namespace Pxf {
namespace Modules
{
    class GenericImageImporter : public Pxf::Module
    {
        private:
        public:
        GenericImageImporter(const char* _Identifier, unsigned _KernelVersion, unsigned _ApiVersion)
            : Module(_Identifier, _KernelVersion, _ApiVersion)
        {}
        
        virtual bool RegisterSystem(Pxf::Kernel* _Kernel, unsigned _SystemType);
    };
    
}}

#endif // _PXF_MODULES_IMG_H_