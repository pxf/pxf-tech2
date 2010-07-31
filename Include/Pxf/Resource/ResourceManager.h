#ifndef _PXF_RESOURCE_RESOURCEMANAGER_H_
#define _PXF_RESOURCE_RESOURCEMANAGER_H_

namespace Pxf {
namespace Resource
{
    class ResourceManager
    {
    private:
    public:
        template <typename ResourceType>
        ResourceType* AcquireResource(const char* _FilePath, unsigned _Options);
    };
} // Resource
} // Pxf

#endif // _PXF_RESOURCE_RESOURCEMANAGER_H_