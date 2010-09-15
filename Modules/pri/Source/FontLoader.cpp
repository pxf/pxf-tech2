#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Stream.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Resource/Chunk.h>
#include <Pxf/Kernel.h>
#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/Image.h>
#include <Pxf/Modules/pri/FontLoader.h>
#include <Pxf/Base/Memory.h>

#define LOCAL_MSG "FontLoader"

using namespace Pxf;
using namespace Modules;

bool BitmapFont::Build()
{
	// Set constant values
	const uint32 HEADER		   = ByteswapNtoL(0xBADC0FFE);	
	const uint32 SECTION_INFO	 = ByteswapNtoL(0x53C70001);
	const uint32 SECTION_COMMON   = ByteswapNtoL(0x53C70002);
	const uint32 SECTION_CHARS	= ByteswapNtoL(0x53C70003);
	const uint32 SECTION_KERNINGS = ByteswapNtoL(0x53C70004);
	const uint32 SECTION_IMAGE	= ByteswapNtoL(0x53C70005);

	// Check chunk
	if (m_Chunk->size < 512) // Appropriate value? Anyone? Hm. :/
	{
		Message("BitmapFont", "Chunk to small to be a font.");
		return false;
	}

	// Setup memory stream
	MemoryStream data((char*)m_Chunk->data, m_Chunk->size);

	// Check header
	if (data.ReadLE<uint32>() != HEADER)
	{
		Message("BitmapFont", "invalid header :-(");
		return false;
	}

	// Info header
	if (data.ReadLE<uint32>() != SECTION_INFO)
	{
		Message("BitmapFont", "expected info section");
		return false;
	}

	// Read font name
	uint32 fontname_length = data.ReadLE<uint32>();
	data.Skip(fontname_length); // fix

	// Common header
	if (data.ReadLE<uint32>() != SECTION_COMMON)
	{
		Message("BitmapFont", "expected common section");
		return false;
	}

	m_Width = data.ReadLE<int16>();
	m_Height = data.ReadLE<int16>();
	m_LineHeight = data.ReadLE<int16>();

	// Chars header
	if (data.ReadLE<uint32>() != SECTION_CHARS)
	{
		Message("BitmapFont", "expected character section");
		return false;
	}

	int16 num_chars = data.ReadLE<int16>();

	if (!m_CharInfo)
		m_CharInfo = new CharInfo_t[256];

	if (!m_CharInfo)
	{
		Message("BitmapFont", "Unable allocate memory for character data");
		return false;
	}

	MemorySet(m_CharInfo, 0, sizeof(CharInfo_t) * 256);

	for(int i = 0; i < num_chars; i++)
	{
		int16 id = data.ReadLE<int16>();
		m_CharInfo[id].Width	= float(data.ReadLE<int16>());
		m_CharInfo[id].Height   = float(data.ReadLE<int16>());
		m_CharInfo[id].XOffset  = float(data.ReadLE<int16>());
		m_CharInfo[id].YOffset  = float(data.ReadLE<int16>());
		m_CharInfo[id].XAdvance = float(data.ReadLE<int16>());
		m_CharInfo[id].Tx1	  = data.ReadLE<float>();
		m_CharInfo[id].Ty1	  = data.ReadLE<float>();
		m_CharInfo[id].Tx2	  = data.ReadLE<float>();
		m_CharInfo[id].Ty2	  = data.ReadLE<float>();
	}

	if (data.ReadLE<uint32>() != SECTION_KERNINGS)
	{
		Message("BitmapFont", "missing kerning section");
		return false;
	}

	int num_kernings = data.ReadLE<int16>();
	if (num_kernings > 0)
	{
		if (m_Kernings)
			SafeDelete(m_Kernings);

		m_Kernings = new Kerning_t[num_kernings];

		MemorySet(m_Kernings, 0, sizeof(Kerning_t) * num_kernings);
		for(int i = 0; i < num_kernings; i++)
		{
			m_Kernings[i].first  = (char)data.ReadLE<int16>();
			m_Kernings[i].second = (char)data.ReadLE<int16>();
			m_Kernings[i].amount = data.ReadLE<int16>();
		}
	}


	if (data.ReadLE<uint32>() != SECTION_IMAGE)
	{
		Message("BitmapFont", "missing image section (lol..)");
		return false;
	}

	int image_size = data.ReadLE<uint32>(); // not used for now though..

	if (image_size != data.GetRemainingBytes())
	{
		Message("BitmapFont", "wrong image size. corrupt data?");
		return false;
	}

	// Create chunk and read image
	Resource::Chunk* chunk = new Resource::Chunk();

	if (!chunk)
	{
		Message("BitmapFont", "could not allocate chunk, out of memory");
		return false;
	}

	chunk->data = data.GetDataPointer();
	chunk->size = data.GetRemainingBytes();
	chunk->is_static = true;

	Resource::ResourceManager* mgr = m_Kernel->GetResourceManager();
	Resource::ImageLoader* ldr = mgr->FindResourceLoader<Resource::ImageLoader>("png");

	if (m_Image)
		ldr->Destroy(m_Image);

	m_Image = ldr->CreateFrom(chunk->data, chunk->size);

	if (!m_Image)
	{
		Message("BitmapFont", "could not allocate image, out of memory");
		SafeDelete(chunk);
		return false;
	}

	if (!m_Image->IsReady())
	{
		Message("BitmapFont", "invalid image in font");
		return false;
	}

	return true;

}

BitmapFont::~BitmapFont()
{
	SafeDelete(m_Image);
	SafeDelete(m_Chunk);
	SafeDelete(m_CharInfo);
	SafeDelete(m_Kernings);
}

BitmapFontLoader::BitmapFontLoader(Pxf::Kernel* _Kernel)
	: FontLoader(_Kernel, "Bitmap Font Loader")
{
	Init();
}

bool BitmapFontLoader::Init()
{
	return true;
}

Resource::Font* BitmapFontLoader::Load(const char* _FilePath)
{
	Resource::Chunk* chunk = Resource::LoadFile(_FilePath);				   
	if (!chunk)
	{
		Message("FontLoader", "Unable to create chunk from file '%s'", _FilePath);
		return NULL;
	}
	return new BitmapFont(m_Kernel, chunk, this);
}

Resource::Font* BitmapFontLoader::CreateFrom(const void* _DataPtr, unsigned _DataLen)
{
	Resource::Chunk* chunk = new Resource::Chunk();
	chunk->data = (void*) _DataPtr;
	chunk->size = _DataLen;
	chunk->is_static = true;
	return new BitmapFont(m_Kernel, chunk, this);
}

BitmapFontLoader::~BitmapFontLoader()
{
}