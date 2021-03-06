#include "TexturedQuadBatch.h"

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

#include <Pxf/Graphics/GraphicsDevice.h>

using namespace Fabric;
using namespace Pxf::Graphics;
using namespace Pxf::Math;

TexturedQuadBatch::TexturedQuadBatch(unsigned int _size, const char* _texture_filepath, float* _currentdepth, Pxf::Math::Vec4f* _currentcolor, Pxf::Math::Mat4* _transformmatrix, bool _linear) :
    QuadBatch(_size, _currentdepth, _currentcolor, _transformmatrix)
{
    m_TextureFilepath = _texture_filepath;
    
    // Load texture
    m_Texture = Pxf::Kernel::GetInstance()->GetGraphicsDevice()->CreateTexture(m_TextureFilepath);
		if (_linear)
		{
			m_Texture->SetMagFilter(TEX_FILTER_LINEAR);
	    m_Texture->SetMinFilter(TEX_FILTER_LINEAR);
		}
		else
		{
			m_Texture->SetMagFilter(TEX_FILTER_NEAREST);
	    m_Texture->SetMinFilter(TEX_FILTER_NEAREST);
		}
    m_Texture->SetClampMethod(TEX_REPEAT);//TEX_CLAMP_TO_EDGE);
}

TexturedQuadBatch::TexturedQuadBatch(unsigned int _size, int _width, int _height, int _channels, const unsigned char* _texture_data, float* _currentdepth, Pxf::Math::Vec4f* _currentcolor, Pxf::Math::Mat4* _transformmatrix, bool _linear) :
    QuadBatch(_size, _currentdepth, _currentcolor, _transformmatrix)
{
    //m_TextureFilepath = _texture_filepath;
    
    // Load texture
    m_Texture = Pxf::Kernel::GetInstance()->GetGraphicsDevice()->CreateTextureFromData(_texture_data, _width, _height, _channels);
		if (_linear)
		{
			m_Texture->SetMagFilter(TEX_FILTER_LINEAR);
	    m_Texture->SetMinFilter(TEX_FILTER_LINEAR);
		}
		else
		{
			m_Texture->SetMagFilter(TEX_FILTER_NEAREST);
	    m_Texture->SetMinFilter(TEX_FILTER_NEAREST);
		}
    m_Texture->SetClampMethod(TEX_CLAMP_TO_EDGE);
}

TexturedQuadBatch::TexturedQuadBatch(unsigned int _size, int _width, int _height, int _channels, Pxf::Graphics::Texture* _texture, float* _currentdepth, Pxf::Math::Vec4f* _currentcolor, Pxf::Math::Mat4* _transformmatrix, bool _linear) :
    QuadBatch(_size, _currentdepth, _currentcolor, _transformmatrix)
{
    //m_TextureFilepath = _texture_filepath;
    
    // Load texture
		m_Texture = _texture;
    //m_Texture = Pxf::Kernel::GetInstance()->GetGraphicsDevice()->CreateTextureFromData(_texture_data, _width, _height, _channels);
		if (_linear)
		{
			m_Texture->SetMagFilter(TEX_FILTER_LINEAR);
	    m_Texture->SetMinFilter(TEX_FILTER_LINEAR);
		}
		else
		{
			m_Texture->SetMagFilter(TEX_FILTER_NEAREST);
	    m_Texture->SetMinFilter(TEX_FILTER_NEAREST);
		}
    m_Texture->SetClampMethod(TEX_CLAMP_TO_EDGE);
}

TexturedQuadBatch::~TexturedQuadBatch()
{
  Pxf::Kernel::GetInstance()->GetGraphicsDevice()->DestroyTexture(m_Texture);
}

void TexturedQuadBatch::SetTextureSubset(float tl_u, float tl_v, float br_u, float br_v)
{
  Vec4f coords = m_Texture->CreateTextureSubset(tl_u, tl_v, br_u, br_v);
  QuadBatch::SetTextureSubset(coords.x, coords.y, coords.z, coords.w);
}

void TexturedQuadBatch::Reset()
{
    QuadBatch::Reset();
}

void TexturedQuadBatch::Begin()
{
    QuadBatch::Begin();
}

void TexturedQuadBatch::End()
{
    QuadBatch::End();
}

void TexturedQuadBatch::Draw()
{
    Texture* oldtex = Pxf::Kernel::GetInstance()->GetGraphicsDevice()->BindTexture(m_Texture);
    QuadBatch::Draw();
    Pxf::Kernel::GetInstance()->GetGraphicsDevice()->BindTexture(oldtex);
}