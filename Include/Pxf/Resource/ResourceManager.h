#ifndef _PXF_RESOURCE_RESOURCEMANAGER_H_
#define _PXF_RESOURCE_RESOURCEMANAGER_H_

#include <Pxf/Kernel.h>
#include <Pxf/Base/Logger.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Path.h>
#include <Pxf/Util/Map.h>
#include <Pxf/Resource/ResourceLoader.h>
#include <Pxf/Resource/ResourceBase.h>
#include <Pxf/Resource/Chunk.h>
#include <Pxf/Util/String.h>

// TODO: Handle non-file resources

namespace Pxf {
namespace Resource
{   
	class ResourceBase;
	class ResourceManager
	{
	private:
		Kernel* m_Kernel;
		unsigned m_LogTag;
		Pxf::Util::Map<Util::String, ResourceLoader*>* m_ResourceLoaders;
		Pxf::Util::Map<Util::String, ResourceBase*>* m_LoadedResources;
	public:
		ResourceManager(Kernel* _Kernel);
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
		ResourceType* Reload(const char* _FilePath,const char* _ForcedExt = 0)
		{
			if(!_FilePath) return NULL;

			const char* fileext = _ForcedExt;
			ResourceBase* resource = NULL;

			if (_ForcedExt == 0)
				fileext = PathExt(_FilePath);

			Pxf::Util::Map<Util::String,ResourceBase*>::iterator 
				resit = m_LoadedResources->find(_FilePath);

			if(resit == m_LoadedResources->end())
			{
				m_Kernel->Log(m_LogTag | Logger::IS_WARNING, "Failed to reload resource '%s'", _FilePath);
				return NULL;
			}
			else
			{

				resource = (ResourceBase*) resit->second;
				resource->m_Loader->Destroy(resource);

				//m_LoadedResources->erase(resit);
				//m_LoadedResources->insert(std::make_pair(_FilePath, resource));
			}

			return (ResourceType*)resource;
		}

		template <typename ResourceType>
		ResourceType* Acquire(const char* _FilePath, const char* _ForcedExt = 0)
		{
			ResourceBase* resource = NULL;

			const char* fileext = _ForcedExt;
			if (_ForcedExt == 0)
				fileext = PathExt(_FilePath);
			
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
						m_Kernel->Log(m_LogTag | Logger::IS_CRITICAL, "Failed to load resource '%s'", _FilePath);
						return NULL;
					}
					m_LoadedResources->insert(std::make_pair(_FilePath, resource));
				}
				else
				{
					m_Kernel->Log(m_LogTag | Logger::IS_WARNING, "No supported loader available for type '%s'", fileext);
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
				if (!_Resource->GetSource())
				{
					m_Kernel->Log(m_LogTag | Logger::IS_WARNING, "Removing unmanaged resource (fixme)");
					_Resource->m_Loader->Destroy((ResourceType*)_Resource);
					return;
				}

				_Resource->m_References--;

				if (_Resource->m_References <= 0 || _Purge)
				{
					Util::Map<Util::String, ResourceBase*>::iterator iter = m_LoadedResources->find(_Resource->GetSource());
					if (iter != m_LoadedResources->end())
					{
						m_Kernel->Log(m_LogTag | Logger::IS_INFORMATION, "Purging resource holding '%s' (%s)", _Resource->GetSource(), _Purge? "Forced":"No more refs");
						m_LoadedResources->erase(iter);
					}

					_Resource->m_Loader->Destroy((ResourceType*)_Resource);
				}
				else
					m_Kernel->Log(m_LogTag | Logger::IS_INFORMATION, "Releasing resource holding '%s' [refs = %d]", _Resource->GetSource(), _Resource->m_References);
			}
		}
		
	};
} // Resource
} // Pxf

#endif // _PXF_RESOURCE_RESOURCEMANAGER_H_