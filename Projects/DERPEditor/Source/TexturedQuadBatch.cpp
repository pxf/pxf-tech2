#include "TexturedQuadBatch.h"

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

#include <Pxf/Graphics/Texture.h>
#include <Pxf/Graphics/GraphicsDevice.h>

using namespace DERPEditor;
using namespace Pxf::Graphics;
using namespace Pxf::Math;

TexturedQuadBatch::TexturedQuadBatch(unsigned int _size, Mat4* _transformmatrix, const char* _texture_filepath) :
    QuadBatch(_size, _transformmatrix)
{
    m_TextureFilepath = _texture_filepath;
    
    // Load texture
    m_Texture = Pxf::Kernel::GetInstance()->GetGraphicsDevice()->CreateTexture(m_TextureFilepath);
}

TexturedQuadBatch::~TexturedQuadBatch()
{
    
}

void TexturedQuadBatch::SetTextureSubset(float tl_u, float tl_v, float br_u, float br_v)
{
  Vec4f coords = m_Texture->CreateTextureSubset(tl_u, tl_v, br_u, br_v);
  QuadBatch::SetTextureSubset(coords.x, coords.y, coords.z, coords.w);
  /*
  m_CurrentCoords[0].u = tl_u;
	m_CurrentCoords[0].v = tl_v;

	m_CurrentCoords[1].u = br_u;
	m_CurrentCoords[1].v = tl_v;
             
	m_CurrentCoords[2].u = br_u;
	m_CurrentCoords[2].v = br_v;
             
	m_CurrentCoords[3].u = tl_u;
	m_CurrentCoords[3].v = br_v;
	*/
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