#include "QuadBatch.h"

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Graphics/VertexBufferDefs.h>

using namespace DERPEditor;
using namespace Pxf::Graphics;
using namespace Pxf::Math;

QuadBatch::QuadBatch(int _size)
{
    m_VertBufSize = _size;
    m_VertexBuffer = Pxf::Kernel::GetInstance()->GetGraphicsDevice()->CreateVertexBuffer(VB_LOCATION_SYS, VB_USAGE_DYNAMIC_DRAW);
    m_VertexBuffer->CreateNewBuffer(_size, sizeof(QuadVertex) ); // pos = 3, tex coords = 2, colors = 4
    
    m_VertexBuffer->SetData(VB_VERTEX_DATA, 0, 3); // SetData(Type, OffsetInBytes, NumComponents)
    m_VertexBuffer->SetData(VB_TEXCOORD_DATA, sizeof(Vec3f), 2);
	m_VertexBuffer->SetData(VB_COLOR_DATA, 2*sizeof(Vec3f), 4);
	m_VertexBuffer->SetPrimitive(VB_PRIMITIVE_QUADS);
    
    m_pVertBuf = (QuadVertex*)m_VertexBuffer->MapData(VB_ACCESS_WRITE_ONLY);
    Reset();
}

QuadBatch::~QuadBatch()
{
    Pxf::Kernel::GetInstance()->GetGraphicsDevice()->DestroyVertexBuffer(m_VertexBuffer);
}

void QuadBatch::Reset()
{
    m_CurrentVert = 0;
    
    m_CurrentColor = Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void QuadBatch::Draw()
{
    Pxf::Kernel::GetInstance()->GetGraphicsDevice()->DrawBuffer(m_VertexBuffer, m_CurrentVert);
}

void QuadBatch::AddCentered(float x, float y, float w, float h)
{
    float w2 = w / 2.0f;
    float h2 = h / 2.0f;
    printf("wat\n");
    m_pVertBuf[m_CurrentVert].position = Vec3f(x-w2,y-h2,m_CurrentDepth);
    m_pVertBuf[m_CurrentVert].coord = m_CurrentCoords[0];
    m_pVertBuf[m_CurrentVert].color = m_CurrentColor;
    
    printf("wat\n");
    m_pVertBuf[m_CurrentVert+1].position = Vec3f(x+w2,y-h2,m_CurrentDepth);
    m_pVertBuf[m_CurrentVert+1].coord = m_CurrentCoords[1];
    m_pVertBuf[m_CurrentVert+1].color = m_CurrentColor;
    
    m_pVertBuf[m_CurrentVert+2].position = Vec3f(x+w2,y+h2,m_CurrentDepth);
    m_pVertBuf[m_CurrentVert+2].coord = m_CurrentCoords[2];
    m_pVertBuf[m_CurrentVert+2].color = m_CurrentColor;
    
    m_pVertBuf[m_CurrentVert+3].position = Vec3f(x-w2,y+h2,m_CurrentDepth);
    m_pVertBuf[m_CurrentVert+3].coord = m_CurrentCoords[3];
    m_pVertBuf[m_CurrentVert+3].color = m_CurrentColor;
    
    m_CurrentVert = m_CurrentVert + 4;
}