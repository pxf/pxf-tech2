#include <Pxf/Resource/ResourceManager.h>

using namespace Pxf;

void Resource::ResourceManager::RegisterResourceLoader(Resource::ResourceLoader* _ResourceLoader)
{
    m_ResourceLoaders.push_back(_ResourceLoader);
}