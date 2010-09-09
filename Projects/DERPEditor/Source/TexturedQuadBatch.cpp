#include "TexturedQuadBatch.h"

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

#include <Pxf/Graphics/Texture.h>
#include <Pxf/Graphics/GraphicsDevice.h>

using namespace DERPEditor;
using namespace Pxf::Graphics;
using namespace Pxf::Math;

TexturedQuadBatch::TexturedQuadBatch(unsigned int _size, const char* _texture_filepath, float* _currentdepth, Pxf::Math::Vec4f* _currentcolor, Pxf::Math::Mat4* _transformmatrix) :
    QuadBatch(_size, _currentdepth, _currentcolor, _transformmatrix)
{
    m_TextureFilepath = _texture_filepath;
    
    // Load texture
    m_Texture = Pxf::Kernel::GetInstance()->GetGraphicsDevice()->CreateTexture(m_TextureFilepath);
    m_Texture->SetMagFilter(TEX_FILTER_NEAREST);
    m_Texture->SetMinFilter(TEX_FILTER_NEAREST);
    m_Texture->SetClampMethod(TEX_CLAMP_TO_EDGE);
}

TexturedQuadBatch::~TexturedQuadBatch()
{
  delete m_Texture;
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