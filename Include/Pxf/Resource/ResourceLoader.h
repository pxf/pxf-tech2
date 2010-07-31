#ifndef _PXF_RESOURCE_RESOURCELOADER_H_
#define _PXF_RESOURCE_RESOURCELOADER_H_

#include <Pxf/System.h>

namespace Pxf {
namespace Resource
{
    class Chunk;
    
    template <typename ResourceType>
    class ResourceLoader : public Pxf::System
    {
    private:
        // virtual void Init() = 0;
    public:
        ResourceLoader(Pxf::Kernel* _Kernel, unsigned _Identifier)
            : Pxf::System(_Kernel, Pxf::System::SYSTEM_TYPE_RESOURCE_LOADER, _Identifier)
        {}
        
        ResourceType* Load(Pxf::Resource::Chunk* _Chunk);
    };
} // Resource
} // Pxf

#endif // _PXF_RESOURCE_RESOURCELOADER_H_