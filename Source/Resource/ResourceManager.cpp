#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Resource/ResourceBase.h>

using namespace Pxf;

Resource::ResourceManager::ResourceManager()
{
	m_ResourceLoaders = new Pxf::Util::Map<Util::String, ResourceLoader*>();
	m_LoadedResources = new Pxf::Util::Map<Util::String, ResourceBase*>();
}

Resource::ResourceManager::~ResourceManager()
{
    // clean up resource loaders
    if (m_ResourceLoaders->size() > 0)
    {
        Util::Map<Util::String, ResourceLoader*>::iterator iter;
        for(iter = m_ResourceLoaders->begin(); iter != m_ResourceLoaders->end(); ++iter)
        {
			if (iter->second)
			{
				// TODO: several extentions might share loaders, make sure not to delete multiple times
				// iter->second->Destroy(iter->second);
			}
        }
    }
    // clean up loaded resources
    if (m_LoadedResources->size() > 0)
    {
        Message("ResourceManager", "Cleaning up unreleased resources");
        Util::Map<Util::String, ResourceBase*>::iterator iter;
        for(iter = m_LoadedResources->begin(); iter != m_LoadedResources->end(); ++iter)
        {
            Message("ResourceManager", "Deleting resource '%s' [refs = %d]", iter->second->GetSource(), iter->second->m_References);
            iter->second->m_Loader->Destroy(iter->second);
        }
    }
}

void Resource::ResourceManager::RegisterResourceLoader(const char* _Ext, Resource::ResourceLoader* _ResourceLoader)
{
    m_ResourceLoaders->insert(std::make_pair(_Ext, _ResourceLoader));
}