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


	m_VertexBuffer = GetDevice()->CreateVertexBuffer(VB_LOCATION_GPU, VB_USAGE_DYNAMIC_DRAW);
	
	m_VertexBuffer->CreateNewBuffer(md.triangle_count*3,3*sizeof(float));
	//m_VertexBuffer->CreateFromBuffer((void*) md.vertices,md.triangle_count*3,12);

	int bufSize = md.triangle_count * 9;

	float* buf = new float[bufSize];
	const float* p = md.vertices;

	float* bufp = (float*) m_VertexBuffer->MapData(VB_ACCESS_READ_WRITE);

	m_VertexBuffer->SetData(VB_VERTEX_DATA, 0, 3); // SetData(Type, OffsetInBytes, NumComponents)
	m_VertexBuffer->SetPrimitive(VB_PRIMITIVE_TRIANGLES);

	//Pxf::MemoryCopy(buf,md.vertices,bufSize);

	for(int i = 0; i < bufSize; i++)
	{
		bufp[i] = p[i];

		if( (i % 3) == 0)
			printf("---\n");

		printf("%f\n",bufp[i]);
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