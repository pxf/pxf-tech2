#ifndef _PXF_RESOURCE_IMAGE_H_
#define _PXF_RESOURCE_IMAGE_H_

#include <Pxf/Resource/ResourceBase.h>
#include <Pxf/Resource/ResourceLoader.h>

namespace Pxf {
namespace Resource {
	class Image : public ResourceBase
	{
	protected:
		int m_Width;
		int m_Height;
		int m_Channels;
		unsigned char* m_ImageData;

	public:
		Image(Kernel* _Kernel, Chunk* _Chunk, ResourceLoader* _Loader)
			: ResourceBase(_Kernel, _Chunk, _Loader)
			, m_Channels(0)
			, m_Height(0)
			, m_Width(0)
			, m_ImageData(NULL)
		{}

		Image(Kernel* _Kernel, Chunk* _Chunk, ResourceLoader* _Loader, int _Width, int _Height, int _Channels)
			: ResourceBase(_Kernel, _Chunk, _Loader)
			, m_Channels(_Channels)
			, m_Height(_Height)
			, m_Width(m_Width)
			, m_ImageData(NULL)
		{}

		virtual ~Image()
		{}

		const int Width() const
		{
			return m_Width;
		} 

		const int Height() const
		{
			return m_Height;
		} 

		const int Channels() const
		{
			return m_Channels;
		} 
		
		unsigned char* Ptr() const
		{
			return m_ImageData;
		}

		virtual const bool IsReady() const
		{
			return m_ImageData != NULL;
		}
	};

	class ImageLoader : public Resource::ResourceLoader
	{
	private:
		bool Init(){ return true; }
	public:
		ImageLoader(Pxf::Kernel* _Kernel, const char* _Identifier)
			: ResourceLoader(_Kernel, _Identifier)
		{}
		virtual ~ImageLoader() {};
		virtual Resource::Image* Load(const char* _FilePath) = 0;
		virtual Resource::Image* CreateFrom(const void* _DataPtr, unsigned _DataLen) = 0;
		virtual Resource::Image* CreateFromRaw(int _Width, int _Height, int _Channels, unsigned char* _DataPtr) = 0;
		virtual void Destroy(void* _Resource)
		{
			if (_Resource)
				delete (Resource::Image*)_Resource;
		}
	};

} // Resource
} // Pxf

#endif //_PXF_RESOURCE_IMAGE_H_

