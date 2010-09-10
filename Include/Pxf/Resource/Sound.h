#ifndef _PXF_RESOURCE_SOUND_H_
#define _PXF_RESOURCE_SOUND_H_

#include <Pxf/Resource/ResourceBase.h>
#include <Pxf/Resource/ResourceLoader.h>

namespace Pxf {
namespace Resource {
	class Sound : public ResourceBase
	{
	protected:
		float m_Length;
		int m_Channels;
		short* m_SoundData;
		unsigned m_SoundDataLen;

	public:
		Sound(Chunk* _Chunk, ResourceLoader* _Loader)
            : ResourceBase(_Chunk, _Loader)
            , m_Length(0.f)
            , m_Channels(0)
            , m_SoundData(NULL)
			, m_SoundDataLen(0)
        {}
		virtual ~Sound()
        {}

		const float Length() const
		{
			return m_Length;
		} 

		const unsigned int Channels() const
		{
			return m_Channels;
		} 
		
		short* DataPtr() const
		{
			return m_SoundData;
		}

		unsigned DataLen() const
		{
			return m_SoundDataLen;
		}

		virtual const bool IsReady() const
		{
			return m_SoundData != NULL;
		}
	};

	class SoundLoader : public Resource::ResourceLoader
	{
	private:
		bool Init(){ return true; }
	public:
		SoundLoader(Pxf::Kernel* _Kernel, const char* _Identifier)
			: ResourceLoader(_Kernel, _Identifier)
		{}
		virtual ~SoundLoader() {};
		virtual Resource::Sound* Load(const char* _FilePath) = 0;
		virtual Resource::Sound* CreateFrom(const void* _DataPtr, unsigned _DataLen) = 0;
		virtual void Destroy(void* _Resource)
		{
			if (_Resource)
				delete (Resource::Sound*)_Resource;
		}
	};

} // Resource
} // Pxf

#endif //_PXF_RESOURCE_SOUND_H_

