#include <Pxf/Graphics/PrimitiveBatch.h>

#include <Pxf/Math/Vector.h>
#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Graphics/Texture.h>
#include <Pxf/Graphics/VertexBuffer.h>
#include <Pxf/Resource/Image.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

using namespace Pxf;
using namespace Pxf::Graphics;

PrimitiveBatch::PrimitiveBatch(GraphicsDevice* _pDevice)
	: DeviceResource(_pDevice)
	, m_VertexBuffer(0)
	, m_VertexData(0)
	, m_Rotation(0.0f)
	, m_VertexBufferPos(0)
	, m_CurrentMode(ENone)
{
	m_VertexBuffer = _pDevice->CreateVertexBuffer(VB_LOCATION_SYS, VB_USAGE_STATIC_COPY);

	m_VertexBuffer->CreateNewBuffer(4 * 1024, sizeof(Vertex));
	m_VertexBuffer->SetData(VB_VERTEX_DATA, 0, 3);
	m_VertexBuffer->SetData(VB_TEXCOORD0_DATA, sizeof(Math::Vec3f), 2);
	m_VertexBuffer->SetData(VB_COLOR_DATA, sizeof(Math::Vec2f)+sizeof(Math::Vec3f), 4);

	// set z index of vertices to something low
	m_VertexData = (Vertex*)m_VertexBuffer->MapData(VB_ACCESS_READ_WRITE);
	for (int i = 0; i < m_VertexBuffer->GetVertexCount(); i++)
	{
		m_VertexData[i].pos.z = 0.f;
	}
	m_VertexBuffer->UnmapData();

}

PrimitiveBatch::~PrimitiveBatch()
{
	m_pDevice->DestroyVertexBuffer(m_VertexBuffer);
}

void PrimitiveBatch::SetColor(float r, float g, float b, float a)
{
	m_CurrentColors[0].Set(r, g, b, a);
	m_CurrentColors[1].Set(r, g, b, a);
	m_CurrentColors[2].Set(r, g, b, a);
	m_CurrentColors[3].Set(r, g, b, a);
}

void PrimitiveBatch::SetColor(Math::Vec4f* c)
{
	SetColor(c->r, c->g, c->b, c->a);
}

void PrimitiveBatch::Flush()
{	
	if (m_VertexBufferPos == 0 || m_CurrentMode == ENone)
		return;

	m_pDevice->Translate(Math::Vec3f(0.375, 0.375, 0.));
	switch(m_CurrentMode)
	{
	case EDrawPoints:
		m_VertexBuffer->SetPrimitive(VB_PRIMITIVE_POINTS);
		m_pDevice->DrawBuffer(m_VertexBuffer, m_VertexBufferPos);
		break;
	case EDrawLines:
		// For perfect pixelization...
		m_VertexBuffer->SetPrimitive(VB_PRIMITIVE_LINES);
		m_pDevice->DrawBuffer(m_VertexBuffer, m_VertexBufferPos);
		break;
	case EDrawQuads:
		m_VertexBuffer->SetPrimitive(VB_PRIMITIVE_QUADS);
		m_pDevice->DrawBuffer(m_VertexBuffer, m_VertexBufferPos);
		break;
	default:
		PXF_ASSERT(0, "Invalid drawing mode");
	}
	m_pDevice->Translate(Math::Vec3f(-0.375, -0.375, 0.));
	m_VertexBufferPos = 0;
}


void PrimitiveBatch::PointsBegin()
{
	PXF_ASSERT(m_CurrentMode == ENone, "Drawing or already in another drawing mode");
	m_VertexData = (Vertex*)m_VertexBuffer->MapData(VB_ACCESS_WRITE_ONLY);
	m_CurrentMode = EDrawPoints;
	SetColor(1.f,1.f,1.f,1.f);
}

void PrimitiveBatch::PointsDraw(float x, float y)
{
	PXF_ASSERT(m_CurrentMode == EDrawPoints, "Invalid mode for drawing points");

	m_VertexData[m_VertexBufferPos].pos.x = x;
	m_VertexData[m_VertexBufferPos].pos.y = y;
	m_VertexData[m_VertexBufferPos].tex = m_CurrentTexCoords[0];
	m_VertexData[m_VertexBufferPos].color = m_CurrentColors[0];

	m_VertexBufferPos += 1;
	if (m_VertexBufferPos + 1 >= m_VertexBuffer->GetVertexCount())
		Flush();

}

void PrimitiveBatch::PointsEnd()
{
	PXF_ASSERT(m_CurrentMode == EDrawPoints, "Invalid mode");
	m_VertexBuffer->UnmapData();
	Flush();
	m_CurrentMode = ENone;
}


void PrimitiveBatch::LinesBegin()
{
	PXF_ASSERT(m_CurrentMode == ENone, "Drawing or already in another drawing mode");
	m_VertexData = (Vertex*)m_VertexBuffer->MapData(VB_ACCESS_WRITE_ONLY);
	m_CurrentMode = EDrawLines;
	SetColor(1.f,1.f,1.f,1.f);
}

void PrimitiveBatch::LinesDraw(float x0, float y0, float x1, float y1)
{
	PXF_ASSERT(m_CurrentMode == EDrawLines, "Invalid mode for drawing lines");

	m_VertexData[m_VertexBufferPos].pos.x = x0;
	m_VertexData[m_VertexBufferPos].pos.y = y0;
	m_VertexData[m_VertexBufferPos].tex = m_CurrentTexCoords[0];
	m_VertexData[m_VertexBufferPos].color = m_CurrentColors[0];

	m_VertexData[m_VertexBufferPos+1].pos.x = x1;
	m_VertexData[m_VertexBufferPos+1].pos.y = y1;
	m_VertexData[m_VertexBufferPos+1].tex = m_CurrentTexCoords[1];
	m_VertexData[m_VertexBufferPos+1].color = m_CurrentColors[1];

	m_VertexBufferPos += 2;
	if (m_VertexBufferPos + 2 >= m_VertexBuffer->GetVertexCount())
		Flush();
}
/*
	Try: offset top left pixel by 0.5
	     offset bottom right pixel by -0.3
*/
void PrimitiveBatch::LinesDrawFrame(float x, float y, float w, float h)
{
	LinesDraw(  x,   y, x+w, y);   // roof
	LinesDraw(  x, y+h, x+w, y+h); // floor
	LinesDraw(  x,   y,   x, y+h); // left
	LinesDraw(x+w,   y, x+w, y+h); // right
}

void PrimitiveBatch::LinesEnd()
{
	PXF_ASSERT(m_CurrentMode == EDrawLines, "Invalid mode");
	m_VertexBuffer->UnmapData();
	Flush();
	m_CurrentMode = ENone;
}

void PrimitiveBatch::QuadsBegin()
{
	PXF_ASSERT(m_CurrentMode == ENone, "Drawing or already in another drawing mode");
	m_CurrentMode = EDrawQuads;
	m_VertexData = (Vertex*)m_VertexBuffer->MapData(VB_ACCESS_WRITE_ONLY);
	QuadsSetRotation(0.f);
	QuadsSetTextureSubset(0.f,0.f,1.f,1.f);
	SetColor(1.f,1.f,1.f,1.f);
}

void PrimitiveBatch::QuadsDrawCentered(float x, float y, float w, float h)
{
	QuadsDrawTopLeft(x-w/2, y-h/2, w, h);
}

static void Rotate(float rotation, const Math::Vector3D<float> &center, Math::Vector3D<float> &point)
{
	Math::Vector3D<float> p = point - center;
	point.x = p.x * cosf(rotation) - p.y * sinf(rotation) + center.x;
	point.y = p.x * sinf(rotation) + p.y * cosf(rotation) + center.y;
}

void PrimitiveBatch::QuadsDrawTopLeft(float x, float y, float w, float h)
{
	PXF_ASSERT(m_CurrentMode == EDrawQuads, "Invalid mode for drawing quads");

	Math::Vec3f center(x + w/2, y + h/2, 0.f);

	m_VertexData[m_VertexBufferPos].pos.x = x;
	m_VertexData[m_VertexBufferPos].pos.y = y;
	m_VertexData[m_VertexBufferPos].tex = m_CurrentTexCoords[0];
	m_VertexData[m_VertexBufferPos].color = m_CurrentColors[0];
	Rotate(m_Rotation,center, m_VertexData[m_VertexBufferPos].pos);

	m_VertexData[m_VertexBufferPos+1].pos.x = x+w;
	m_VertexData[m_VertexBufferPos+1].pos.y = y;
	m_VertexData[m_VertexBufferPos+1].tex = m_CurrentTexCoords[1];
	m_VertexData[m_VertexBufferPos+1].color = m_CurrentColors[1];
	Rotate(m_Rotation,center, m_VertexData[m_VertexBufferPos+1].pos);

	m_VertexData[m_VertexBufferPos+2].pos.x = x+w;
	m_VertexData[m_VertexBufferPos+2].pos.y = y+h;
	m_VertexData[m_VertexBufferPos+2].tex = m_CurrentTexCoords[2];
	m_VertexData[m_VertexBufferPos+2].color = m_CurrentColors[2];
	Rotate(m_Rotation,center, m_VertexData[m_VertexBufferPos+2].pos);

	m_VertexData[m_VertexBufferPos+3].pos.x = x;
	m_VertexData[m_VertexBufferPos+3].pos.y = y+h;
	m_VertexData[m_VertexBufferPos+3].tex = m_CurrentTexCoords[3];
	m_VertexData[m_VertexBufferPos+3].color = m_CurrentColors[3];
	Rotate(m_Rotation,center, m_VertexData[m_VertexBufferPos+3].pos);

	m_VertexBufferPos += 4;
	if (m_VertexBufferPos + 4 >= m_VertexBuffer->GetVertexCount())
		Flush();
}

void PrimitiveBatch::QuadsDrawFreeform(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3)
{
	m_VertexData[m_VertexBufferPos].pos.x = x0;
	m_VertexData[m_VertexBufferPos].pos.y = y0;
	m_VertexData[m_VertexBufferPos].tex = m_CurrentTexCoords[3];
	m_VertexData[m_VertexBufferPos].color = m_CurrentColors[3];

	m_VertexData[m_VertexBufferPos+3].pos.x = x1;
	m_VertexData[m_VertexBufferPos+3].pos.y = y1;
	m_VertexData[m_VertexBufferPos+3].tex = m_CurrentTexCoords[3];
	m_VertexData[m_VertexBufferPos+3].color = m_CurrentColors[3];

	m_VertexData[m_VertexBufferPos+3].pos.x = x2;
	m_VertexData[m_VertexBufferPos+3].pos.y = y2;
	m_VertexData[m_VertexBufferPos+3].tex = m_CurrentTexCoords[3];
	m_VertexData[m_VertexBufferPos+3].color = m_CurrentColors[3];

	m_VertexData[m_VertexBufferPos+3].pos.x = x3;
	m_VertexData[m_VertexBufferPos+3].pos.y = y3;
	m_VertexData[m_VertexBufferPos+3].tex = m_CurrentTexCoords[3];
	m_VertexData[m_VertexBufferPos+3].color = m_CurrentColors[3];

	m_VertexBufferPos += 4;
	if (m_VertexBufferPos + 4 >= m_VertexBuffer->GetVertexCount())
		Flush();
}

void PrimitiveBatch::QuadsSetRotation(float angle)
{
	PXF_ASSERT(m_CurrentMode == EDrawQuads, "Invalid mode for rotation");
	m_Rotation = angle;
}

void PrimitiveBatch::QuadsSetTextureSubset(float tl_u, float tl_v, float br_u, float br_v)
{
	PXF_ASSERT(m_CurrentMode == EDrawQuads, "Invalid mode for setting texture subset");

	m_CurrentTexCoords[0].u = tl_u;
	m_CurrentTexCoords[0].v = tl_v;

	m_CurrentTexCoords[1].u = br_u;
	m_CurrentTexCoords[1].v = tl_v;

	m_CurrentTexCoords[2].u = br_u;
	m_CurrentTexCoords[2].v = br_v;

	m_CurrentTexCoords[3].u = tl_u;
	m_CurrentTexCoords[3].v = br_v;
}

void PrimitiveBatch::QuadsEnd()
{
	PXF_ASSERT(m_CurrentMode == EDrawQuads, "Invalid mode");
	m_VertexBuffer->UnmapData();
	Flush();
	m_CurrentMode = ENone;
}
