#ifndef _PXF_RESOURCE_FONT_H_
#define _PXF_RESOURCE_FONT_H_

#include <Pxf/Resource/ResourceBase.h>
#include <Pxf/Resource/ResourceLoader.h>

/*
	TODO: Distance fields for supersmooth font rendering. :)
*/

namespace Pxf {
namespace Resource {
	class Image;
	class Font : public ResourceBase
	{
	public:
		struct CharInfo_t
		{
			float Tx1, Ty1, Tx2, Ty2;
			float Width, Height;
			float XOffset, YOffset;
			float XAdvance;
		};

		struct Kerning_t
		{
			unsigned char first, second;
			short amount;
		};
	protected:
		Image* m_Image;

		float m_Width;
		float m_Height;
		float m_LineHeight;
		int m_NumKernings;

		CharInfo_t* m_CharInfo;
		Kerning_t* m_Kernings;

	public:
		Font(Kernel* _Kernel, Chunk* _Chunk, ResourceLoader* _Loader)
			: ResourceBase(_Kernel, _Chunk, _Loader)
			, m_LineHeight(0.f)
			, m_Width(0.f)
			, m_Height(0.f)
			, m_NumKernings(0)
			, m_CharInfo(0)
			, m_Kernings(0)
			, m_Image(0)
		{}

		virtual ~Font()
		{}

		inline const CharInfo_t* GetInfo(const unsigned char _Char) const
		{
			if (m_CharInfo)
				return &m_CharInfo[_Char];
			return 0;
		}

		inline short GetKernings(const unsigned char _First, const unsigned char _Second) const
		{
			for(int i = 0; i < m_NumKernings; i++)
			{
				if (m_Kernings[i].first == _First && m_Kernings[i].second == _Second)
					return m_Kernings[i].amount;
			}
			return 0;
		}

		inline int GetLineHeight() const
		{
			return m_LineHeight;
		}

		inline Resource::Image* GetImage() const
		{
			return m_Image;
		}

		inline bool HasKernings() const
		{
			return m_Kernings != 0;
		}

		virtual const bool IsReady() const
		{
			return m_CharInfo != NULL;
		}
	};

	class FontLoader : public Resource::ResourceLoader
	{
	private:
		bool Init(){ return true; }
	public:
		FontLoader(Pxf::Kernel* _Kernel, const char* _Identifier)
			: ResourceLoader(_Kernel, _Identifier)
		{}
		virtual ~FontLoader() {};
		virtual Resource::Font* Load(const char* _FilePath) = 0;
		virtual Resource::Font* CreateFrom(const void* _DataPtr, unsigned _DataLen) = 0;
		virtual void Destroy(void* _Resource)
		{
			if (_Resource)
				delete (Resource::Font*)_Resource;
		}
	};

} // Resource
} // Pxf

#endif //_PXF_RESOURCE_FONT_H_

