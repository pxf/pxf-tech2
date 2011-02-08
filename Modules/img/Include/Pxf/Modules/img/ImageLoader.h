#ifndef _PXF_RESOURCE_IMAGELOADER_H_
#define _PXF_RESOURCE_IMAGELOADER_H_

#include <Pxf/Kernel.h>
#include <Pxf/Resource/ResourceLoader.h>
#include <Pxf/Resource/Image.h>


namespace Pxf{
namespace Resource
{
	class Chunk;
}
	
namespace Modules {

	class SOILImage : public Resource::Image
	{
	protected:
		unsigned m_LogTag;
		bool m_IsRaw;
		virtual bool Build();
	public:
		SOILImage(Kernel* _Kernel, Resource::Chunk* _Chunk, Resource::ResourceLoader* _Loader);

		SOILImage(Kernel* _Kernel, Resource::Chunk* _Chunk, Resource::ResourceLoader* _Loader, int _Width, int _Height, int _Channels)
			: Resource::Image(_Kernel, _Chunk, _Loader, _Width, _Height, _Channels)
			, m_IsRaw(true)
		{
			m_ImageData = (unsigned char*)_Chunk->data;
		}
		
		virtual ~SOILImage();

		virtual bool SaveAs(const char* _Filename);
	};

	class GenericImageLoader : public Resource::ImageLoader
	{
	private:
		unsigned m_LogTag;
		bool Init(){ return true; }
	public:
		GenericImageLoader(Pxf::Kernel* _Kernel);
		~GenericImageLoader();
		virtual Resource::Image* Load(const char* _FilePath);
		virtual Resource::Image* CreateFrom(const void* _DataPtr, unsigned _DataLen);
		virtual Resource::Image* CreateFromRaw(int _Width, int _Height, int _Channels, unsigned char* _DataPtr);
	};

} // Graphics
} // Pxf

#endif //_PXF_RESOURCE_IMAGELOADER_H_
