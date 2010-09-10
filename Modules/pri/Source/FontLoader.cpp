#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Modules/pri/FontLoader.h>

#define LOCAL_MSG "FontLoader"

using namespace Pxf;
using namespace Modules;

bool BitmapFont::Build()
{

	return true;
}

BitmapFontLoader::BitmapFontLoader(Pxf::Kernel* _Kernel)
	: FontLoader(_Kernel, "Bitmap Font Loader")
{
	Init();
}

bool BitmapFontLoader::Init()
{
	return false;
}

Resource::Font* BitmapFontLoader::Load(const char* _FilePath)
{
	return 0;
}

Resource::Font* BitmapFontLoader::CreateFrom(const void* _DataPtr, unsigned _DataLen)
{

	return 0;
}

BitmapFontLoader::~BitmapFontLoader()
{
}