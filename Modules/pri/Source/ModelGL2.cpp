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
//	m_VertexBuffer->SetData(VB_NORMAL_DATA, sizeof(Math::Vec3f), 3);
	m_VertexBuffer->SetPrimitive(VB_PRIMITIVE_TRIANGLES);

	ModelGL2::Vertex* ptr = (ModelGL2::Vertex*) m_VertexBuffer->MapData(VB_ACCESS_READ_WRITE);
	for(int i = 0, j = 0; i < md.triangle_count; i++, j+=3)
	{
		unsigned int i1 = md.indices[j+0];
		unsigned int i2 = md.indices[j+1];
		unsigned int i3 = md.indices[j+2];
		Math::Vec3f* v1 = (Math::Vec3f*)&md.vertices[i1];
		Math::Vec3f* v2 = (Math::Vec3f*)&md.vertices[i2];
		Math::Vec3f* v3 = (Math::Vec3f*)&md.vertices[i3];
		ptr[j].vertex = *v1;
		ptr[j+1].vertex = *v2;
		ptr[j+2].vertex = *v3;
		Message("Model", "%d. (%.2f, %.2f, %.2f)", i, ptr[i].vertex.x, ptr[i].vertex.y, ptr[i].vertex.z);
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