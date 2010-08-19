#ifndef _PXF_RESOURCE_RESOURCELOADER_H_
#define _PXF_RESOURCE_RESOURCELOADER_H_

#include <Pxf/System.h>

namespace Pxf {
namespace Resource
{
    class Chunk;
    class ResourceBase;
    
    class ResourceLoader : public Pxf::System
    {
    private:
        // virtual void Init() = 0;
    public:
        ResourceLoader(Pxf::Kernel* _Kernel, const char* _Identifier)
            : Pxf::System(_Kernel, Pxf::System::SYSTEM_TYPE_RESOURCE_LOADER, _Identifier)
        {}

        virtual ResourceBase* Load(const char* _FilePath) = 0;
		virtual ResourceBase* CreateFrom(const void* _DataPtr, unsigned _DataLen) = 0;
        virtual void Destroy(void* _Resource) = 0;
    };
} // Resource
} // Pxf

#endif // _PXF_RESOURCE_RESOURCELOADER_H_