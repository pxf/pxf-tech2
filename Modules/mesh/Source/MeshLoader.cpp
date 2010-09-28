#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Memory.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Modules/mesh/MeshLoader.h>

#include <Pxf/Kernel.h>
#include <Pxf/Base/Logger.h>

using namespace Pxf;
using namespace Modules;

OpenCTMMesh::OpenCTMMesh(Kernel* _Kernel, Resource::Chunk* _Chunk, Resource::ResourceLoader* _Loader)
	: Resource::Mesh(_Kernel, _Chunk,_Loader)
	, m_LogTag(0)
{
	m_LogTag = m_Kernel->CreateTag("res");
}

bool OpenCTMMesh::Build()
{
	return true;
}

/*
void OpenCTMMesh::SetData(unsigned int _VertCount, unsigned int _TriCount,const float* _Vertices,const unsigned int* _Indices,const float* _Normals)
{
	m_MeshData.vertex_count = _VertCount;
	m_MeshData.triangle_count = _TriCount;
	m_MeshData.vertices = _Vertices;
	m_MeshData.normals = _Normals;
	m_MeshData.indices= _Indices;

	if(_Normals)
		m_MeshData.has_normals = true;
}*/

CtmMeshLoader::CtmMeshLoader(Pxf::Kernel* _Kernel)
	: MeshLoader(_Kernel, "Ctm Mesh Loader")
	, m_LogTag(0)
{
	m_LogTag = m_Kernel->CreateTag("res");
	Init();
}

bool CtmMeshLoader::Init()
{
	m_Context = ctmNewContext(CTM_IMPORT);

	if(ctmGetError(m_Context) == CTM_NONE)	
	{
		return true;
	}
	else
	{
		m_Kernel->Log(m_LogTag | Logger::IS_CRITICAL, "Unable to create OpenCTM Context");
		return false;
	}
}

Resource::Mesh* CtmMeshLoader::Load(const char* _FilePath)
{
	ctmLoad(m_Context, _FilePath);
	Resource::Chunk* _Chunk = Resource::LoadFile(_FilePath);   

	// something failed during load
	if(!_Chunk)
	{
		m_Kernel->Log(m_LogTag | Logger::IS_CRITICAL, "Error loading model %s",_FilePath);
		return 0;
	}

	// something is wrong with context
	if(ctmGetError(m_Context) == CTM_NONE)
	{
		// no errors found
		CTMuint _VertCount			= 0;
		CTMuint	_TriCount			= 0;
		int	_UVMapCount				= 0;
		const CTMuint*	_Indices	= 0;
		const CTMfloat* _Vertices	= 0;
		const CTMfloat* _Normals	= 0;
		bool _HasNormals			= false;

		_VertCount = ctmGetInteger(m_Context, CTM_VERTEX_COUNT);
		_Vertices = ctmGetFloatArray(m_Context, CTM_VERTICES);
		_TriCount = ctmGetInteger(m_Context, CTM_TRIANGLE_COUNT);
		_Indices = ctmGetIntegerArray(m_Context, CTM_INDICES);
		_UVMapCount = ctmGetInteger(m_Context, CTM_UV_MAP_COUNT);

		if(ctmGetInteger(m_Context, CTM_HAS_NORMALS) == CTM_TRUE)
		{
			_HasNormals = true;
			_Normals = ctmGetFloatArray(m_Context,CTM_NORMALS);
		}

		OpenCTMMesh* _NewMesh = new OpenCTMMesh(m_Kernel, _Chunk,this);

		Resource::Mesh::mesh_descriptor _Data;
		_Data.has_normals = _HasNormals;
		_Data.vertex_count = _VertCount;
		_Data.triangle_count = _TriCount;

		float *n_vertices, *n_normals;
		unsigned int* n_indices;

		n_vertices = (float *) MemoryAllocate(3 * sizeof(float) * _VertCount);
		MemoryCopy(n_vertices, _Vertices, 3 * sizeof(float)*_VertCount);
		if (_HasNormals)
		{
			n_normals = (float *) MemoryAllocate(3 * sizeof(float) * _VertCount);
			MemoryCopy(n_normals, _Normals, 3 * sizeof(float)*_VertCount);
		}
		n_indices = (unsigned int *) MemoryAllocate(3 * sizeof(unsigned int) * _TriCount);
		MemoryCopy(n_indices, _Indices, 3 * sizeof(unsigned int)*_TriCount);

		_Data.vertices = n_vertices;
		_Data.normals = n_normals;
		_Data.indices= n_indices;

		m_Kernel->Log(m_LogTag | Logger::IS_INFORMATION, "Finished loading model %s", _FilePath);

		_NewMesh->SetData(_Data);

		return _NewMesh;
	}
	else
	{
		m_Kernel->Log(m_LogTag | Logger::IS_CRITICAL, "Error loading model %s",_FilePath);
		delete _Chunk;
		return 0;
	}
}

Resource::Mesh* CtmMeshLoader::CreateFrom(const void* _DataPtr, unsigned _DataLen)
{
	// LOL DUN NO !!!
	return 0;
}

CtmMeshLoader::~CtmMeshLoader()
{
	ctmFreeContext(m_Context);
}