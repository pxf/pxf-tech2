#include <Pxf/Modules/pri/ModelGL2.h>
//#include <Pxf/Modules/mesh/
#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/Mesh.h>
#include <Pxf/Kernel.h>
#include <Pxf/Base/Memory.h>

#define LOCAL_MSG "ModelGL2"

using namespace Pxf;
using namespace Modules;
using namespace Graphics;

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
		Message(LOCAL_MSG, "Load failed");
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
		m_VertexBuffer->SetData(VB_NORMAL_DATA, sizeof(Math::Vec3f), 3);
	m_VertexBuffer->SetPrimitive(VB_PRIMITIVE_TRIANGLES);

	ModelGL2::Vertex* ptr = (ModelGL2::Vertex*) m_VertexBuffer->MapData(VB_ACCESS_READ_WRITE);
	for(int i = 0; i < md.triangle_count*3; i++)
	{
		unsigned int idx = md.indices[i] * 3;
		
		Math::Vec3f vertex = Math::Vec3f(*(md.vertices+idx+0), *(md.vertices+idx+1), *(md.vertices+idx+2));
		Math::Vec3f normal;
		if (md.has_normals)
			normal = Math::Vec3f(*(md.normals+idx+0), *(md.normals+idx+1), *(md.normals+idx+2));

		// set position
		ptr[i].vertex = vertex;
		ptr[i].normal = normal;



		Message("Model", "%d. [%d](%.2f, %.2f, %.2f)", i, idx, ptr[i].vertex.x, ptr[i].vertex.y, ptr[i].vertex.z);
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