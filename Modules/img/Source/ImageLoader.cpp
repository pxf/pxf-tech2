#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Path.h>
#include <Pxf/Base/String.h>
#include <Pxf/Modules/img/ImageLoader.h>
#include <Pxf/Resource/Chunk.h>

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
	if(m_ImageData && !m_IsRaw)
		SOIL_free_image_data(m_ImageData);
}

bool Modules::SOILImage::SaveAs(const char* _Filename)
{
	const char* ext = PathExt(_Filename);
	int save_type = SOIL_SAVE_TYPE_TGA;
	if (StringCompare(ext, "tga") == 0)
		SOIL_SAVE_TYPE_TGA;
	else if (StringCompare(ext, "dds") == 0)
		SOIL_SAVE_TYPE_DDS;
	else if (StringCompare(ext, "bmp") == 0)
		SOIL_SAVE_TYPE_BMP;
	else
		Message("Image", "Save using .bmp, .dds or .tga. '%s' is not supported", ext);
	return SOIL_save_image(_Filename, save_type, m_Width, m_Height, m_Channels, m_ImageData);
}

Modules::GenericImageLoader::GenericImageLoader(Pxf::Kernel* _Kernel)
	: ImageLoader(_Kernel, "Generic Image Loader")
{
}

Modules::GenericImageLoader::~GenericImageLoader()
{
}

Resource::Image* Modules::GenericImageLoader::Load(const char* _FilePath)
{
	Resource::Chunk* chunk = Resource::LoadFile(_FilePath);				   
	if (!chunk)
	{
		Message("ImageLoader", "Unable to create chunk from file '%s'", _FilePath);
		return NULL;
	}
	return new SOILImage(m_Kernel, chunk, this);
}

Resource::Image* Modules::GenericImageLoader::CreateFrom(const void* _DataPtr, unsigned _DataLen)
{
	Resource::Chunk* chunk = new Resource::Chunk();
	chunk->data = (void*) _DataPtr;
	chunk->size = _DataLen;
	chunk->is_static = true;
	return new SOILImage(m_Kernel, chunk, this);
}

Resource::Image* Modules::GenericImageLoader::CreateFromRaw(int _Width, int _Height, int _Channels, unsigned char* _DataPtr)
{
	Resource::Chunk* chunk = new Resource::Chunk();
	chunk->data = (void*) _DataPtr;
	chunk->size = _Width * _Height * _Channels;
	chunk->is_static = false;
	return new SOILImage(m_Kernel, chunk, this, _Width, _Height, _Channels);
}