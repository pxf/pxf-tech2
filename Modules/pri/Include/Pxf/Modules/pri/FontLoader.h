#ifndef _PXF_RESOURCE_FONTLOADER_H_
#define _PXF_RESOURCE_FONTLOADER_H_

#include <Pxf/Kernel.h>
#include <Pxf/Resource/ResourceLoader.h>
#include <Pxf/Resource/Font.h>

namespace Pxf{
	
namespace Modules {

	class BitmapFont : public Resource::Font
	{
	protected:
		unsigned m_LogTag;
		virtual bool Build();
	public:
		BitmapFont(Kernel* _Kernel, Resource::Chunk* _Chunk, Resource::ResourceLoader* _Loader);
		
		virtual ~BitmapFont();
	};

	class BitmapFontLoader : public Resource::FontLoader
	{
	private:
		unsigned m_LogTag;
		bool Init();
	public:
		BitmapFontLoader(Pxf::Kernel* _Kernel);
		virtual ~BitmapFontLoader();
		virtual Resource::Font* Load(const char* _FilePath);
		virtual Resource::Font* CreateFrom(const void* _DataPtr, unsigned _DataLen);
	};

} // Graphics
} // Pxf

#endif //_PXF_RESOURCE_FONTLOADER_H_
