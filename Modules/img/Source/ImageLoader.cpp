#include <Pxf/Base/Debug.h>
#include <Pxf/Modules/img/ImageLoader.h>

using namespace Pxf;

Modules::GenericImageLoader::GenericImageLoader(Pxf::Kernel* _Kernel)
    : ResourceLoader(_Kernel, "Generic Image Loader")
{

}

Modules::GenericImageLoader::~GenericImageLoader()
{

}

void* Modules::GenericImageLoader::Load(Resource::Chunk* _Chunk)
{
    return 0;
}