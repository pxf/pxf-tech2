#include <Pxf/Resource/ResourceManager.h>

using namespace Pxf;

Resource::ResourceManager::ResourceManager()
{}

Resource::ResourceManager::~ResourceManager()
{
    // clean up resource loaders
    // -> several extentions might share loaders, make sure not to delete multiple times

    // clean up loaded resources
}

void Resource::ResourceManager::RegisterResourceLoader(const char* _Ext, Resource::ResourceLoader* _ResourceLoader)
{
    m_ResourceLoaders.insert(std::make_pair(_Ext, _ResourceLoader));
}