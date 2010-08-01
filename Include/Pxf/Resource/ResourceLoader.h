#ifndef _PXF_RESOURCE_RESOURCELOADER_H_
#define _PXF_RESOURCE_RESOURCELOADER_H_

#include <Pxf/System.h>

namespace Pxf {
namespace Resource
{
    class Chunk;
    
    class ResourceLoader : public Pxf::System
    {
    private:
        // virtual void Init() = 0;
    public:
        ResourceLoader(Pxf::Kernel* _Kernel, const char* _Identifier)
            : Pxf::System(_Kernel, Pxf::System::SYSTEM_TYPE_RESOURCE_LOADER, _Identifier)
        {}

        virtual void* Load(Pxf::Resource::Chunk* _Chunk) = 0;
    };
} // Resource
} // Pxf

#endif // _PXF_RESOURCE_RESOURCELOADER_H_