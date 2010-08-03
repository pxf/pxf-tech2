#ifndef _PXF_RESOURCE_IMAGE_H_
#define _PXF_RESOURCE_IMAGE_H_

#include <Pxf/Resource/ResourceBase.h>

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
		Image(Chunk* _Chunk, ResourceLoader* _Loader)
            : ResourceBase(_Chunk, _Loader)
            , m_Channels(0)
            , m_Height(0)
            , m_Width(0)
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

} // Resource
} // Pxf

#endif //_PXF_RESOURCE_IMAGE_H_

