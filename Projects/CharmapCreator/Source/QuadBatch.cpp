#include "QuadBatch.h"

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Graphics/VertexBufferDefs.h>

using namespace Pxf::Graphics;
using namespace Pxf::Math;

QuadBatch::QuadBatch(unsigned int _size, float* _currentdepth, Vec4f* _currentcolor, Mat4* _transformmatrix)
{
    m_VertBufSize = _size;
    m_CurrentColor = _currentcolor;
    m_CurrentDepth = _currentdepth;
    m_Transformation = _transformmatrix;
    
    m_VertexBuffer = Pxf::Kernel::GetInstance()->GetGraphicsDevice()->CreateVertexBuffer(VB_LOCATION_GPU, VB_USAGE_DYNAMIC_DRAW);
    m_VertexBuffer->CreateNewBuffer(_size, sizeof(QuadVertex) ); // pos = 3, tex coords = 2, colors = 4
    
    m_VertexBuffer->SetData(VB_VERTEX_DATA, 0, 3); // SetData(Type, OffsetInBytes, NumComponents)
	m_VertexBuffer->SetData(VB_COLOR_DATA, sizeof(Vec3f), 4);
	m_VertexBuffer->SetData(VB_TEXCOORD0_DATA, sizeof(Vec3f)+sizeof(Vec4f), 2);
	m_VertexBuffer->SetPrimitive(VB_PRIMITIVE_QUADS);
    
    Reset();
}

QuadBatch::~QuadBatch()
{
    Pxf::Kernel::GetInstance()->GetGraphicsDevice()->DestroyVertexBuffer(m_VertexBuffer);
}

void QuadBatch::Reset()
{
    m_CurrentVert = 0;
    
    //m_CurrentColor = Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
    //m_CurrentDepth = 0.0f;
    
    //SetTextureSubset(0.f,0.f,1.f,1.f);
    ResetTextureSubset();
}

void QuadBatch::ResetTextureSubset()
{
  QuadBatch::SetTextureSubset(0.f,0.f,1.f,1.f);
}

void QuadBatch::SetTextureSubset(float tl_u, float tl_v, float br_u, float br_v)
{
  m_CurrentCoords[0].u = tl_u;
	m_CurrentCoords[0].v = tl_v;

	m_CurrentCoords[1].u = br_u;
	m_CurrentCoords[1].v = tl_v;
             
	m_CurrentCoords[2].u = br_u;
	m_CurrentCoords[2].v = br_v;
             
	m_CurrentCoords[3].u = tl_u;
	m_CurrentCoords[3].v = br_v;
}

void QuadBatch::Begin()
{
    m_pVertBuf = (QuadVertex*)m_VertexBuffer->MapData(VB_ACCESS_WRITE_ONLY);
}

void QuadBatch::End()
{
    m_VertexBuffer->UnmapData();
}

void QuadBatch::Draw()
{
    Pxf::Kernel::GetInstance()->GetGraphicsDevice()->DrawBuffer(m_VertexBuffer, m_CurrentVert);
}

void QuadBatch::AddFreeform(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3)
{
    Vec3f p0,p1,p2,p3;
    if (m_Transformation != NULL) {
        Vec4f p0_,p1_,p2_,p3_;
        p0_ = *m_Transformation * Vec4f(x0,y0, 0.0f, 1.0f);
        p1_ = *m_Transformation * Vec4f(x1,y1, 0.0f, 1.0f);
        p2_ = *m_Transformation * Vec4f(x2,y2, 0.0f, 1.0f);
        p3_ = *m_Transformation * Vec4f(x3,y3, 0.0f, 1.0f);
        p0 = Vec3f(p0_.x, p0_.y, *m_CurrentDepth);
        p1 = Vec3f(p1_.x, p1_.y, *m_CurrentDepth);
        p2 = Vec3f(p2_.x, p2_.y, *m_CurrentDepth);
        p3 = Vec3f(p3_.x, p3_.y, *m_CurrentDepth);
    } else {
        p0 = Vec3f(x0,y0,*m_CurrentDepth);
        p1 = Vec3f(x1,y1,*m_CurrentDepth);
        p2 = Vec3f(x2,y2,*m_CurrentDepth);
        p3 = Vec3f(x3,y3,*m_CurrentDepth);
    }
    
    m_pVertBuf[m_CurrentVert].position = p0;
    m_pVertBuf[m_CurrentVert].color = *m_CurrentColor;
    m_pVertBuf[m_CurrentVert].coord = m_CurrentCoords[0];
    
    m_pVertBuf[m_CurrentVert+1].position = p1;
    m_pVertBuf[m_CurrentVert+1].color = *m_CurrentColor;
    m_pVertBuf[m_CurrentVert+1].coord = m_CurrentCoords[1];
    
    m_pVertBuf[m_CurrentVert+2].position = p2;
    m_pVertBuf[m_CurrentVert+2].color = *m_CurrentColor;
    m_pVertBuf[m_CurrentVert+2].coord = m_CurrentCoords[2];
    
    m_pVertBuf[m_CurrentVert+3].position = p3;
    m_pVertBuf[m_CurrentVert+3].color = *m_CurrentColor;
    m_pVertBuf[m_CurrentVert+3].coord = m_CurrentCoords[3];
    
    m_CurrentVert += 4;
}

void QuadBatch::AddTopLeft(float x, float y, float w, float h)
{
    AddFreeform(x, y, x+w, y, x+w, y+h, x, y+h);
}

void QuadBatch::AddCentered(float x, float y, float w, float h)
{
    float w2 = w / 2.0f;
    float h2 = h / 2.0f;
    
    AddFreeform(x-w2, y-h2, x+w2, y-h2, x+w2, y+h2, x-w2, y+h2);
}
