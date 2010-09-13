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
		virtual bool Build();
	public:
		SOILImage(Kernel* _Kernel, Resource::Chunk* _Chunk, Resource::ResourceLoader* _Loader)
			: Resource::Image(_Kernel, _Chunk, _Loader)
		{
			Build();
		}
		
		virtual ~SOILImage();
	};

	class GenericImageLoader : public Resource::ImageLoader
	{
	private:
		bool Init(){ return true; }
	public:
		GenericImageLoader(Pxf::Kernel* _Kernel);
		~GenericImageLoader();
		virtual Resource::Image* Load(const char* _FilePath);
		virtual Resource::Image* CreateFrom(const void* _DataPtr, unsigned _DataLen);
	};

} // Graphics
} // Pxf

#endif //_PXF_RESOURCE_IMAGELOADER_H_
