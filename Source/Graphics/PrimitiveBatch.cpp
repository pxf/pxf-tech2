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

enum EMode { ENone = 0, EDrawPoints, EDrawLines, EDrawQuads };
static EMode g_CurrentMode = ENone;

struct Vertex
{
	Math::Vec3f pos;
	Math::Vec2f tex;
	Math::Vec4f color;
};

const static int g_BufferSize = 32 * 1024;
static int g_VertexBufferPos = 0;

static Vertex* g_Vertices;
static Math::Vec4f g_CurrentColors[4];
static Math::Vec2f g_CurrentTexCoords[4];
static float g_Rotation = 0.f;

PrimitiveBatch::PrimitiveBatch(GraphicsDevice* _pDevice)
	: DeviceResource(_pDevice)
	, m_VertexBuffer(0)
{
	m_VertexBuffer = _pDevice->CreateVertexBuffer(VB_LOCATION_GPU, VB_USAGE_STREAM_COPY);

	m_VertexBuffer->CreateNewBuffer(4 * 1024, sizeof(Vertex));
	m_VertexBuffer->SetData(VB_VERTEX_DATA, 0, 3);
	m_VertexBuffer->SetData(VB_TEXCOORD_DATA, sizeof(Math::Vec3f), 2);
	m_VertexBuffer->SetData(VB_COLOR_DATA, sizeof(Math::Vec2f)+sizeof(Math::Vec3f), 4);

	if (!g_Vertices)
		g_Vertices = new Vertex[g_BufferSize];
	g_VertexBufferPos = 0;
	g_CurrentMode = ENone;
	g_Rotation = 0.f;

	// set z index of vertices to something low
	for (int i = 0; i < g_BufferSize; i++)
		g_Vertices->pos.z = 0.f;

}

PrimitiveBatch::~PrimitiveBatch()
{
	SafeDeleteArray(g_Vertices);
}

void PrimitiveBatch::SetColor(float r, float g, float b, float a)
{
	g_CurrentColors[0].Set(r, g, b, a);
	g_CurrentColors[1].Set(r, g, b, a);
	g_CurrentColors[2].Set(r, g, b, a);
	g_CurrentColors[3].Set(r, g, b, a);
}

void PrimitiveBatch::SetColor(Math::Vec4f* c)
{
	SetColor(c->r, c->g, c->b, c->a);
}

void Flush(Graphics::GraphicsDevice* _Device, VertexBuffer* _VertexBuffer)
{	
	if (g_VertexBufferPos == 0 || g_CurrentMode == ENone)
		return;

	_VertexBuffer->UpdateData(g_Vertices, g_VertexBufferPos*sizeof(Vertex), 0);

	switch(g_CurrentMode)
	{
	case EDrawPoints:
		_VertexBuffer->SetPrimitive(VB_PRIMITIVE_POINTS);
		_Device->DrawBuffer(_VertexBuffer, g_VertexBufferPos);
		break;
	case EDrawLines:
		// For perfect pixelization...
		//glTranslatef(0.375, 0.375, 0.);
		_VertexBuffer->SetPrimitive(VB_PRIMITIVE_LINES);
		_Device->DrawBuffer(_VertexBuffer, g_VertexBufferPos);
		break;
	case EDrawQuads:
		_VertexBuffer->SetPrimitive(VB_PRIMITIVE_QUADS);
		_Device->DrawBuffer(_VertexBuffer, g_VertexBufferPos);
		break;
	default:
		PXF_ASSERT(0, "Invalid drawing mode");
	}

	g_VertexBufferPos = 0;
}


void PrimitiveBatch::PointsBegin()
{
	PXF_ASSERT(g_CurrentMode == ENone, "Drawing or already in another drawing mode");
	g_CurrentMode = EDrawPoints;
	SetColor(1.f,1.f,1.f,1.f);
}

void PrimitiveBatch::PointsDraw(float x, float y)
{
	PXF_ASSERT(g_CurrentMode == EDrawPoints, "Invalid mode for drawing points");

	g_Vertices[g_VertexBufferPos].pos.x = x;
	g_Vertices[g_VertexBufferPos].pos.y = y;
	g_Vertices[g_VertexBufferPos].tex = g_CurrentTexCoords[0];
	g_Vertices[g_VertexBufferPos].color = g_CurrentColors[0];

	g_VertexBufferPos += 1;
	if (g_VertexBufferPos + 1 >= g_BufferSize)
		Flush(GetDevice(), m_VertexBuffer);

}

void PrimitiveBatch::PointsEnd()
{
	PXF_ASSERT(g_CurrentMode == EDrawPoints, "Invalid mode");
	Flush(GetDevice(), m_VertexBuffer);
	g_CurrentMode = ENone;
}


void PrimitiveBatch::LinesBegin()
{
	PXF_ASSERT(g_CurrentMode == ENone, "Drawing or already in another drawing mode");
	g_CurrentMode = EDrawLines;
	SetColor(1.f,1.f,1.f,1.f);
}

void PrimitiveBatch::LinesDraw(float x0, float y0, float x1, float y1)
{
	PXF_ASSERT(g_CurrentMode == EDrawLines, "Invalid mode for drawing lines");

	g_Vertices[g_VertexBufferPos].pos.x = x0;
	g_Vertices[g_VertexBufferPos].pos.y = y0;
	g_Vertices[g_VertexBufferPos].tex = g_CurrentTexCoords[0];
	g_Vertices[g_VertexBufferPos].color = g_CurrentColors[0];

	g_Vertices[g_VertexBufferPos+1].pos.x = x1;
	g_Vertices[g_VertexBufferPos+1].pos.y = y1;
	g_Vertices[g_VertexBufferPos+1].tex = g_CurrentTexCoords[1];
	g_Vertices[g_VertexBufferPos+1].color = g_CurrentColors[1];

	g_VertexBufferPos += 2;
	if (g_VertexBufferPos + 2 >= g_BufferSize)
		Flush(GetDevice(), m_VertexBuffer);
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
	PXF_ASSERT(g_CurrentMode == EDrawLines, "Invalid mode");
	Flush(GetDevice(), m_VertexBuffer);
	g_CurrentMode = ENone;
}

void PrimitiveBatch::QuadsBegin()
{
	PXF_ASSERT(g_CurrentMode == ENone, "Drawing or already in another drawing mode");
	g_CurrentMode = EDrawQuads;
	QuadsSetRotation(0.f);
	QuadsSetTextureSubset(0.f,0.f,1.f,1.f);
	SetColor(1.f,1.f,1.f,1.f);
}

void PrimitiveBatch::QuadsDrawCentered(float x, float y, float w, float h)
{
	QuadsDrawTopLeft(x-w/2, y-h/2, w, h);
}

static void Rotate(const Math::Vector3D<float> &center, Math::Vector3D<float> &point)
{
	Math::Vector3D<float> p = point - center;
	point.x = p.x * cosf(g_Rotation) - p.y * sinf(g_Rotation) + center.x;
	point.y = p.x * sinf(g_Rotation) + p.y * cosf(g_Rotation) + center.y;
}

void PrimitiveBatch::QuadsDrawTopLeft(float x, float y, float w, float h)
{
	PXF_ASSERT(g_CurrentMode == EDrawQuads, "Invalid mode for drawing quads");

	Math::Vec3f center(x + w/2, y + h/2, 0.f);

	g_Vertices[g_VertexBufferPos].pos.x = x;
	g_Vertices[g_VertexBufferPos].pos.y = y;
	g_Vertices[g_VertexBufferPos].tex = g_CurrentTexCoords[0];
	g_Vertices[g_VertexBufferPos].color = g_CurrentColors[0];
	Rotate(center, g_Vertices[g_VertexBufferPos].pos);

	g_Vertices[g_VertexBufferPos+1].pos.x = x+w;
	g_Vertices[g_VertexBufferPos+1].pos.y = y;
	g_Vertices[g_VertexBufferPos+1].tex = g_CurrentTexCoords[1];
	g_Vertices[g_VertexBufferPos+1].color = g_CurrentColors[1];
	Rotate(center, g_Vertices[g_VertexBufferPos+1].pos);

	g_Vertices[g_VertexBufferPos+2].pos.x = x+w;
	g_Vertices[g_VertexBufferPos+2].pos.y = y+h;
	g_Vertices[g_VertexBufferPos+2].tex = g_CurrentTexCoords[2];
	g_Vertices[g_VertexBufferPos+2].color = g_CurrentColors[2];
	Rotate(center, g_Vertices[g_VertexBufferPos+2].pos);

	g_Vertices[g_VertexBufferPos+3].pos.x = x;
	g_Vertices[g_VertexBufferPos+3].pos.y = y+h;
	g_Vertices[g_VertexBufferPos+3].tex = g_CurrentTexCoords[3];
	g_Vertices[g_VertexBufferPos+3].color = g_CurrentColors[3];
	Rotate(center, g_Vertices[g_VertexBufferPos+3].pos);

	g_VertexBufferPos += 4;
	if (g_VertexBufferPos + 4 >= g_BufferSize)
		Flush(GetDevice(), m_VertexBuffer);
}

void PrimitiveBatch::QuadsDrawFreeform(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3)
{
	g_Vertices[g_VertexBufferPos].pos.x = x0;
	g_Vertices[g_VertexBufferPos].pos.y = y0;
	g_Vertices[g_VertexBufferPos].tex = g_CurrentTexCoords[3];
	g_Vertices[g_VertexBufferPos].color = g_CurrentColors[3];

	g_Vertices[g_VertexBufferPos+3].pos.x = x1;
	g_Vertices[g_VertexBufferPos+3].pos.y = y1;
	g_Vertices[g_VertexBufferPos+3].tex = g_CurrentTexCoords[3];
	g_Vertices[g_VertexBufferPos+3].color = g_CurrentColors[3];

	g_Vertices[g_VertexBufferPos+3].pos.x = x2;
	g_Vertices[g_VertexBufferPos+3].pos.y = y2;
	g_Vertices[g_VertexBufferPos+3].tex = g_CurrentTexCoords[3];
	g_Vertices[g_VertexBufferPos+3].color = g_CurrentColors[3];

	g_Vertices[g_VertexBufferPos+3].pos.x = x3;
	g_Vertices[g_VertexBufferPos+3].pos.y = y3;
	g_Vertices[g_VertexBufferPos+3].tex = g_CurrentTexCoords[3];
	g_Vertices[g_VertexBufferPos+3].color = g_CurrentColors[3];

	g_VertexBufferPos += 4;
	if (g_VertexBufferPos + 4 >= g_BufferSize)
		Flush(GetDevice(), m_VertexBuffer);
}

void PrimitiveBatch::QuadsSetRotation(float angle)
{
	PXF_ASSERT(g_CurrentMode == EDrawQuads, "Invalid mode for rotation");
	g_Rotation = angle;
}

void PrimitiveBatch::QuadsSetTextureSubset(float tl_u, float tl_v, float br_u, float br_v)
{
	PXF_ASSERT(g_CurrentMode == EDrawQuads, "Invalid mode for setting texture subset");

	g_CurrentTexCoords[0].u = tl_u;
	g_CurrentTexCoords[0].v = tl_v;

	g_CurrentTexCoords[1].u = br_u;
	g_CurrentTexCoords[1].v = tl_v;

	g_CurrentTexCoords[2].u = br_u;
	g_CurrentTexCoords[2].v = br_v;

	g_CurrentTexCoords[3].u = tl_u;
	g_CurrentTexCoords[3].v = br_v;
}

void PrimitiveBatch::QuadsEnd()
{
	PXF_ASSERT(g_CurrentMode == EDrawQuads, "Invalid mode");
	Flush(GetDevice(), m_VertexBuffer);
	g_CurrentMode = ENone;
}
