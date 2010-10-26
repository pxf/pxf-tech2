#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Resource/ResourceBase.h>
#include <Pxf/Resource/Blob.h>
#include <Pxf/Resource/Text.h>

using namespace Pxf;

Resource::ResourceManager::ResourceManager(Kernel* _Kernel)
	: m_Kernel(_Kernel)
	, m_LogTag(0)
{
	m_LogTag = m_Kernel->CreateTag("res");
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

	m_Kernel->Log(m_LogTag | Logger::IS_INFORMATION, "Destroying resource manager");

	// clean up loaded resources
	if (m_LoadedResources->size() > 0)
	{
		m_Kernel->Log(m_LogTag | Logger::IS_INFORMATION, "Cleaning up unreleased resources");
		Util::Map<Util::String, ResourceBase*>::iterator iter;
		for(iter = m_LoadedResources->begin(); iter != m_LoadedResources->end(); ++iter)
		{
			m_Kernel->Log(m_LogTag | Logger::IS_INFORMATION, "Deleting resource '%s' [refs = %d]", iter->first.c_str(), iter->second->m_References);
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
	m_Kernel->Log(m_LogTag | Logger::IS_INFORMATION, "Enumerating resource loaders...");
	int i = 0;
	for(Util::Map<Util::String, ResourceLoader*>::iterator it = m_ResourceLoaders->begin(); it != m_ResourceLoaders->end(); ++it)
	{
		m_Kernel->Log(m_LogTag | Logger::IS_INFORMATION, "| %d. %s -> %s", i++, it->first.c_str(), it->second->GetIdentifier());
	}
}