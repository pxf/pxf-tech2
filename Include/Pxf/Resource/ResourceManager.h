#ifndef _PXF_RESOURCE_RESOURCEMANAGER_H_
#define _PXF_RESOURCE_RESOURCEMANAGER_H_

#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Path.h>
#include <Pxf/Util/Map.h>
#include <Pxf/Resource/ResourceLoader.h>
#include <Pxf/Resource/ResourceBase.h>
#include <Pxf/Resource/Chunk.h>
#include <Pxf/Util/String.h>

namespace Pxf {
namespace Resource
{   
    class ResourceBase;
    class ResourceManager
    {
    private:
        Pxf::Util::Map<Util::String, ResourceLoader*>* m_ResourceLoaders;
        Pxf::Util::Map<Util::String, ResourceBase*>* m_LoadedResources;
    public:
        ResourceManager();
        ~ResourceManager();
        
        void RegisterResourceLoader(const char* _Ext, Resource::ResourceLoader* _ResourceLoader);

		void DumpResourceLoaders();

		template <typename T>
		T* FindResourceLoader(const char* _Ext)
		{
			Pxf::Util::Map<Util::String, ResourceLoader*>::iterator
				loaderit = m_ResourceLoaders->find(_Ext);
			if (loaderit != m_ResourceLoaders->end())
				return (T*)loaderit->second;
			else
				return NULL;
		}

        template <typename ResourceType>
        ResourceType* Acquire(const char* _FilePath)
        {
            ResourceBase* resource = NULL;
            const char* fileext = PathExt(_FilePath);
            
            Pxf::Util::Map<Util::String, ResourceBase*>::iterator
                resit = m_LoadedResources->find(_FilePath);
            
            if (resit == m_LoadedResources->end())
            {
                // Not found
                Pxf::Util::Map<Util::String, ResourceLoader*>::iterator
                loaderit = m_ResourceLoaders->find(fileext);
                
                if (loaderit != m_ResourceLoaders->end())
                {
                    resource = (ResourceType*)loaderit->second->Load(_FilePath);
                    if (!resource)
                    {
                        Pxf::Message("ResourceManager", "Failed to load resource '%s'", _FilePath);
                        return NULL;
                    }
					m_LoadedResources->insert(std::make_pair(_FilePath, resource));
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
        }
        
		// TODO: just use ResourceBase* _Resource...
        template <typename ResourceType>
        void Release(ResourceType* _Resource, bool _Purge = false)
        {
            if (_Resource)
            {
                _Resource->m_References--;

                if (_Resource->m_References <= 0 || _Purge)
                {
                    Util::Map<Util::String, ResourceBase*>::iterator iter = m_LoadedResources->find(_Resource->GetSource());
                    if (iter != m_LoadedResources->end())
                    {
                        Pxf::Message("ResourceManager", "Purging resource holding '%s' (%s)", _Resource->GetSource(), _Purge? "Forced":"No more refs");
                        m_LoadedResources->erase(iter);
                    }

                    _Resource->m_Loader->Destroy((ResourceType*)_Resource);
                }
                else
                    Message("ResourceManager", "Releasing resource holding '%s' [refs = %d]", _Resource->GetSource(), _Resource->m_References);
            }
        }
        
    };
} // Resource
} // Pxf

#endif // _PXF_RESOURCE_RESOURCEMANAGER_H_