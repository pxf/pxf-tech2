#include <Pxf/Modules/pri/ModelGL2.h>
//#include <Pxf/Modules/mesh/
#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/Mesh.h>
#include <Pxf/Kernel.h>

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

	Resource::Mesh::mesh_descriptor md = (*mesh->GetData());

	m_VertexCount = md.vertex_count;

	m_VertexBuffer = GetDevice()->CreateVertexBuffer(VB_LOCATION_GPU, VB_USAGE_DYNAMIC_DRAW);
	m_VertexBuffer->CreateFromBuffer((void*) md.vertices,md.vertex_count,3);

	m_VertexBuffer->SetData(VB_VERTEX_DATA, 0, 3); // SetData(Type, OffsetInBytes, NumComponents)
	m_VertexBuffer->SetPrimitive(VB_PRIMITIVE_TRIANGLES);

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
	if(m_VertexCount <= 0)
		return;

	GetDevice()->DrawBuffer(m_VertexBuffer, m_VertexCount);
}