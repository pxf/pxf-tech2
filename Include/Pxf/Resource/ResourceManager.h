#ifndef _PXF_RESOURCE_RESOURCEMANAGER_H_
#define _PXF_RESOURCE_RESOURCEMANAGER_H_

#include <Pxf/Base/Path.h>
#include <Pxf/Util/Map.h>
#include <Pxf/Resource/ResourceLoader.h>
#include <Pxf/Resource/Chunk.h>
#include <Pxf/Util/String.h>

namespace Pxf {
namespace Resource
{   
    class ResourceBase;
    class ResourceManager
    {
    private:
        // Todo, need to use String-class, since map.find uses ==
        Pxf::Util::Map<Util::String, ResourceLoader*> m_ResourceLoaders;
        Pxf::Util::Map<Util::String, ResourceBase*> m_LoadedResources;
    public:
        ResourceManager();
        ~ResourceManager();
        
        void RegisterResourceLoader(const char* _Ext, Resource::ResourceLoader* _ResourceLoader);
    
        template <typename ResourceType>
        ResourceType* Acquire(const char* _FilePath, unsigned _Options)
        {
            ResourceBase* resource = NULL;
            const char* fileext = PathExt(_FilePath);
            
            Pxf::Util::Map<Util::String, ResourceBase*>::iterator
                resit = m_LoadedResources.find(_FilePath);
            
            if (resit == m_LoadedResources.end())
            {
                // Not found
                Pxf::Util::Map<Util::String, ResourceLoader*>::iterator
                loaderit = m_ResourceLoaders.find(fileext);
                
                if (loaderit != m_ResourceLoaders.end())
                {
                    resource = (ResourceType*)loaderit->second->Load(_FilePath);
                    if (!resource)
                    {
                        Message("ResourceManager", "Failed to load resource '%s'", _FilePath);
                        return NULL;
                    }
                }
                else
                {
                    Message("ResourceManager", "No supported loader available for type '%s'", fileext);
                    return NULL;
                }
            }
            else
            {
                // Found
                resource = resit->second;
            }
            
            resource->m_References++;
            return (ResourceType*)resource;
            
        /*
            if resource in loaded_resources:
                return loaded_resources[resource]
            else
                ext = get_file_ext(_FilePath)
                foreach loader in loaders:
                    if loader.HasSupportFor(ext)
                        chunk = create_chunk(_FilePath)
                        return (ResourceType*)loader->Load(chunk);
                        break
        */
        }
        
        template <typename ResourceType>
        void Release(ResourceType* _Resource)
        {
            //delete _Resource;
            _Resource->m_Loader->Destroy(_Resource);
        }
        
    };
} // Resource
} // Pxf

#endif // _PXF_RESOURCE_RESOURCEMANAGER_H_