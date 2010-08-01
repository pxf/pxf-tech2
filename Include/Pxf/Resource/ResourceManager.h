#ifndef _PXF_RESOURCE_RESOURCEMANAGER_H_
#define _PXF_RESOURCE_RESOURCEMANAGER_H_

#include <Pxf/Util/Map.h>
#include <Pxf/Util/Array.h>
#include <Pxf/Resource/ResourceLoader.h>

namespace Pxf {
namespace Resource
{   
    class ResourceBase;
    class ResourceManager
    {
    private:
        Pxf::Util::Array<ResourceLoader*> m_ResourceLoaders;
        Pxf::Util::Map<const char*, ResourceBase*> m_LoadedResources;
    public:
    
        void RegisterResourceLoader(Resource::ResourceLoader* _ResourceLoader);
    
        template <typename ResourceType>
        ResourceType* AcquireResource(const char* _FilePath, unsigned _Options)
        {
        /*
            if resource in loaded_resources:
                return loaded_resources[resource]
            else
                ext = get_file_ext(_FilePath)
                foreach loader in loaders:
                    if loader.HasSupportFor(ext)
                        chunk = create_chunk(_FilePath)
                        return loader->Load(chunk);
                        break
        */
        }
    };
} // Resource
} // Pxf

#endif // _PXF_RESOURCE_RESOURCEMANAGER_H_