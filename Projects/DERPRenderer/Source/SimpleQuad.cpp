#include "SimpleQuad.h"

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

#include <Pxf/Graphics/GraphicsDevice.h>

using namespace Derp;
using namespace Pxf;
using namespace Graphics;
using namespace Math;

SimpleQuad::SimpleQuad(float x1, float y1, float x2, float y2)
{
	m_QuadVB = Pxf::Kernel::GetInstance()->GetGraphicsDevice()->CreateVertexBuffer(VB_LOCATION_GPU, VB_USAGE_DYNAMIC_DRAW);
  m_QuadVB->CreateNewBuffer(4, sizeof(QuadVertex) );
  m_QuadVB->SetData(VB_VERTEX_DATA, 0, 3); // SetData(Type, OffsetInBytes, NumComponents)
	m_QuadVB->SetData(VB_COLOR_DATA, sizeof(Vec3f), 4);
	m_QuadVB->SetData(VB_TEXCOORD_DATA, sizeof(Vec3f)+sizeof(Vec4f), 2);
	m_QuadVB->SetPrimitive(VB_PRIMITIVE_QUADS);
	
	Math::Vec2f tCurrentCoords[4];
  tCurrentCoords[0].u = 0.0f;
	tCurrentCoords[0].v = 0.0f;
	tCurrentCoords[1].u = 1.0f;
	tCurrentCoords[1].v = 0.0f;
	tCurrentCoords[2].u = 1.0f;
	tCurrentCoords[2].v = 1.0f;
	tCurrentCoords[3].u = 0.0f;
	tCurrentCoords[3].v = 1.0f;
	
	QuadVertex* pVertBuf;
	pVertBuf = (QuadVertex*)m_QuadVB->MapData(VB_ACCESS_WRITE_ONLY);
	
	pVertBuf[0].position = Vec3f(x1, y1, 0.0f);
  pVertBuf[0].color = Vec4f(1.0f);
  pVertBuf[0].coord = tCurrentCoords[0];
  
  pVertBuf[1].position = Vec3f(x2, y1, 0.0f);
  pVertBuf[1].color = Vec4f(1.0f);
  pVertBuf[1].coord = tCurrentCoords[1];
  
  pVertBuf[2].position = Vec3f(x2, y2, 0.0f);
  pVertBuf[2].color = Vec4f(1.0f);
  pVertBuf[2].coord = tCurrentCoords[2];
  
  pVertBuf[3].position = Vec3f(x1, y2, 0.0f);
  pVertBuf[3].color = Vec4f(1.0f);
  pVertBuf[3].coord = tCurrentCoords[3];
	
	m_QuadVB->UnmapData();
}

SimpleQuad::~SimpleQuad()
{
	Pxf::Kernel::GetInstance()->GetGraphicsDevice()->DestroyVertexBuffer(m_QuadVB);
}

void SimpleQuad::Draw()
{
	Pxf::Kernel::GetInstance()->GetGraphicsDevice()->DrawBuffer(m_QuadVB, 0);
}