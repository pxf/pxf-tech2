#include <Pxf/Graphics/Font.h>
#include <Pxf/Resource/Font.h>

#include <Pxf/Graphics/Texture.h>
#include <Pxf/Graphics/TextureDefs.h>
#include <Pxf/Graphics/PrimitiveBatch.h>

#include <Pxf/Resource/Image.h>
#include <Pxf/Base/Utils.h>


#include <cstring>

using namespace Pxf;
using namespace Pxf::Util;

Graphics::Font::Font(Graphics::GraphicsDevice* _pDevice, const Resource::Font* _Font)
	: m_Font(0)
	, m_Texture(0)
	, DeviceResource(_pDevice)
{

	if (!_Font)
		return;

	m_Font = _Font;
	//m_Texture = new Texture(m_Font->m_Image, 0);
	Resource::Image* fnt = m_Font->GetImage();
	m_Texture = GetDevice()->CreateTexture(fnt);

	if (m_Texture->IsValid())
	{
		m_Texture->SetMagFilter(TEX_FILTER_LINEAR);
		m_Texture->SetMinFilter(TEX_FILTER_NEAREST);
	}

}

Graphics::Font::~Font()
{
	SafeDelete(m_Texture);
}

bool Graphics::Font::IsReady() const
{
	return m_Texture && m_Font && m_Font->GetImage()->IsReady();
}

float Graphics::Font::LineHeight() const
{
	return m_Font->GetLineHeight();
}

// TODO: This method should be shared with Graphics::PrintF
void Graphics::Font::MeasureString(const char* _Text, float *_Width, float *_Height, int _Max)
{
	Math::Vec2f res;
	int len = strlen(_Text);
	if (_Max == -1) _Max = len;
	else if (_Max < len) len = _Max;

	const Resource::Font::CharInfo_t* info;

	float tmp_width = 0;
	float width = 0;
	float height = ceilf(m_Font->GetLineHeight());

	char last_char = 0;
	for (int i = 0; i < len; i++)
	{
		if (_Text[i] == '\n')
		{
			height += ceilf(LineHeight());
			if (tmp_width > width)
				width = tmp_width;
			tmp_width = 0;
		}
		else if (_Text[i] == '\t')
		{
			info = m_Font->GetInfo(' ');
			tmp_width += 2*info->Width;
		}
		else if (!(_Text[i] >= 0 && _Text[i] < 32))
		{
			info = m_Font->GetInfo(_Text[i]);
			int kerning = m_Font->GetKernings(last_char, _Text[i]);
			tmp_width += info->XAdvance + kerning;
		}
	}

	if (tmp_width > width)
		width = tmp_width;

	if(_Width)
		*_Width = width;
	if(_Height)
		*_Height = height;
}