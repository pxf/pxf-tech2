#include <Pxf/Modules/pri/ModelGL2.h>
#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/Mesh.h>
#include <Pxf/Kernel.h>
#include <Pxf/Base/Memory.h>
#include <Pxf/Base/Logger.h>

using namespace Pxf;
using namespace Modules;
using namespace Graphics;

ModelGL2::ModelGL2(Graphics::GraphicsDevice* _pDevice)
	: Graphics::Model(_pDevice)
	, m_TriangleCount(0)
	, m_LogTag(0)
{
	m_LogTag = _pDevice->GetKernel()->CreateTag("gfx");
	if(!Init())
		_pDevice->GetKernel()->Log(m_LogTag | Logger::IS_CRITICAL, "Unable to initialize model");
}

bool ModelGL2::Init()
{

	return true;
}

bool ModelGL2::Load(const char* _FilePath)
{
	Resource::ResourceManager* res = GetDevice()->GetKernel()->GetResourceManager();
	Resource::Mesh* mesh =  res->Acquire<Resource::Mesh>(_FilePath);

	if(!mesh)
	{
		m_pDevice->GetKernel()->Log(m_LogTag | Logger::IS_CRITICAL, "Failed to load model: '%s'", _FilePath);
		return false;
	}

	return Load(mesh);
}

bool ModelGL2::Load(Resource::Mesh* _Mesh)
{
	Resource::Mesh::mesh_descriptor md = (*_Mesh->GetData());

	m_VertexCount = md.vertex_count;
	m_TriangleCount = md.triangle_count;

	m_VertexBuffer = GetDevice()->CreateVertexBuffer(VB_LOCATION_GPU, VB_USAGE_STATIC_DRAW);
	m_VertexBuffer->CreateNewBuffer(md.triangle_count*3, sizeof(ModelGL2::Vertex));
	m_VertexBuffer->SetData(VB_VERTEX_DATA, 0                  , 3); // SetData(Type, OffsetInBytes, NumComponents)
	
	if (md.has_normals)
	{
		m_VertexBuffer->SetData(VB_NORMAL_DATA, sizeof(Math::Vec3f), 3);
	}
	
	if (md.has_uvmap)
	{
		m_VertexBuffer->SetData(VB_TEXCOORD_DATA, sizeof(Math::Vec3f)*2, 2);
	}
	
	m_VertexBuffer->SetPrimitive(VB_PRIMITIVE_TRIANGLES);

	ModelGL2::Vertex* ptr = (ModelGL2::Vertex*) m_VertexBuffer->MapData(VB_ACCESS_READ_WRITE);
	for(int i = 0; i < md.triangle_count*3; i++)
	{
		unsigned int idx = md.indices[i] * 3;
		unsigned int idy = md.indices[i] * 2;
		
		Math::Vec3f vertex = Math::Vec3f(*(md.vertices+idx+0), *(md.vertices+idx+1), *(md.vertices+idx+2));
		Math::Vec3f normal;
		Math::Vec2f texcoord;
		if (md.has_normals)
			normal = Math::Vec3f(*(md.normals+idx+0), *(md.normals+idx+1), *(md.normals+idx+2));

		if (md.has_uvmap)
			texcoord = Math::Vec2f(*(md.texcoords+idy+0), *(md.texcoords+idy+1));

		// set position
		ptr[i].vertex = vertex;
		ptr[i].normal = normal;
		ptr[i].texcoord = texcoord;
	}

	m_VertexBuffer->UnmapData();

	return true;
}

ModelGL2::~ModelGL2()
{
	if (m_VertexBuffer)
		Unload();
}

bool ModelGL2::Unload()
{
	m_VertexCount = 0;
	Pxf::Kernel::GetInstance()->GetGraphicsDevice()->DestroyVertexBuffer(m_VertexBuffer);
	m_VertexBuffer = 0;
	return true;
}

void ModelGL2::Draw()
{
	GetDevice()->DrawBuffer(m_VertexBuffer, m_TriangleCount*3);
}