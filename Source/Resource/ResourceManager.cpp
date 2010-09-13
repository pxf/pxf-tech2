#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Resource/ResourceBase.h>

#include <Pxf/Kernel.h>
#include <Pxf/Resource/Blob.h>
#include <Pxf/Resource/Text.h>

using namespace Pxf;

Resource::ResourceManager::ResourceManager()
{
	m_ResourceLoaders = new Pxf::Util::Map<Util::String, ResourceLoader*>();
	m_LoadedResources = new Pxf::Util::Map<Util::String, ResourceBase*>();

	RegisterResourceLoader("blob", new Resource::BlobLoader(Pxf::Kernel::GetInstance()));
	RegisterResourceLoader("txt", new Resource::TextLoader(Pxf::Kernel::GetInstance()));
}

Resource::ResourceManager::~ResourceManager()
{
	// clean up resource loaders
	// several extensions might share resource loader, but they are located together.
	// we use prev to make sure to only delete a pointer once.

	Message("ResourceManager", "Destroying resource manager");

	// clean up loaded resources
	if (m_LoadedResources->size() > 0)
	{
		Message("ResourceManager", "Cleaning up unreleased resources");
		Util::Map<Util::String, ResourceBase*>::iterator iter;
		for(iter = m_LoadedResources->begin(); iter != m_LoadedResources->end(); ++iter)
		{
			Message("ResourceManager", "Deleting resource '%s' [refs = %d]", iter->first.c_str(), iter->second->m_References);
			iter->second->m_Loader->Destroy(iter->second);
		}
	}

	if (m_ResourceLoaders->size() > 0)
	{
		Util::Map<Util::String, ResourceLoader*>::iterator iter;
		for(iter = m_ResourceLoaders->begin(); iter != m_ResourceLoaders->end(); ++iter)
		{
			if (iter->second)
				iter->second->Destroy(iter->second);
		}
	}
}

void Resource::ResourceManager::RegisterResourceLoader(const char* _Ext, Resource::ResourceLoader* _ResourceLoader)
{
	m_ResourceLoaders->insert(std::make_pair(_Ext, _ResourceLoader));
}

void Resource::ResourceManager::DumpResourceLoaders()
{
	Message("ResourceManager", "Enumerating resource loaders...");
	for(Util::Map<Util::String, ResourceLoader*>::iterator it = m_ResourceLoaders->begin(); it != m_ResourceLoaders->end(); ++it)
	{
		Message("Loader", "| .%s -> %s", it->first.c_str(), it->second->GetIdentifier());
	}
}