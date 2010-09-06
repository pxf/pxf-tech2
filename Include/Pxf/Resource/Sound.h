#ifndef _PXF_RESOURCE_SOUND_H_
#define _PXF_RESOURCE_SOUND_H_

#include <Pxf/Resource/ResourceBase.h>

namespace Pxf {
namespace Resource {
	class Sound : public ResourceBase
	{
	protected:
		unsigned int m_Length;
		unsigned int m_Channels;
		unsigned char* m_SoundData;

	public:
		Sound(Chunk* _Chunk, ResourceLoader* _Loader)
            : ResourceBase(_Chunk, _Loader)
            , m_Length(0)
            , m_Channels(0)
            , m_SoundData(NULL)
        {}
		virtual ~Sound()
        {}

		const unsigned int Length() const
		{
			return m_Length;
		} 

		const unsigned int Channels() const
		{
			return m_Channels;
		} 
		
		unsigned char* Ptr() const
		{
			return m_SoundData;
		}

		virtual const bool IsReady() const
		{
			return m_SoundData != NULL;
		}
	};

} // Resource
} // Pxf

#endif //_PXF_RESOURCE_SOUND_H_

