#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Modules/img/ImageLoader.h>

#include <SOIL.h>

using namespace Pxf;

bool Modules::SOILImage::Build()
{
    if(m_ImageData)
        SOIL_free_image_data(m_ImageData);

    // load raw data into memory
    m_ImageData = SOIL_load_image_from_memory(reinterpret_cast<const unsigned char*>(m_Chunk->data), m_Chunk->size, &m_Width, &m_Height, &m_Channels, 0);

    if (!m_ImageData)
    {
        const char* error = SOIL_last_result();
        Message("Image", "failed to create image from '%s'", m_Chunk->source);
        Message("Image", error);
        return false;
    }
    return true;
}
Modules::SOILImage::~SOILImage()
{
    if(m_ImageData)
        SOIL_free_image_data(m_ImageData);
    
    SafeDelete(m_Chunk);
}

Modules::GenericImageLoader::GenericImageLoader(Pxf::Kernel* _Kernel)
    : ResourceLoader(_Kernel, "Generic Image Loader")
{

}

Modules::GenericImageLoader::~GenericImageLoader()
{

}

void* Modules::GenericImageLoader::Load(Resource::Chunk* _Chunk)
{
    return (void*) new SOILImage(_Chunk);
}