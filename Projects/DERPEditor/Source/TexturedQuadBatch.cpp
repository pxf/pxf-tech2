#include "TexturedQuadBatch.h"

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

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
    //Pxf::Kernel::GetInstance()->GetGraphicsDevice()->BindTexture(m_Texture);
    QuadBatch::Draw();
    //Pxf::Kernel::GetInstance()->GetGraphicsDevice()->BindTexture(0);
}