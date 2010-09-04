#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Modules/mesh/MeshLoader.h>

#define LOCAL_MSG "MeshLoader"

using namespace Pxf;
using namespace Modules;

bool OpenCTMMesh::Build()
{

	return true;
}

void OpenCTMMesh::SetData(unsigned int _VertCount, unsigned int _TriCount,const float* _Vertices,const unsigned int* _Indices,const float* _Normals)
{
	m_MeshData.vertex_count = _VertCount;
	m_MeshData.triangle_count = _TriCount;
	m_MeshData.vertices = _Vertices;
	m_MeshData.normals = _Normals;
	m_MeshData.indices= _Indices;

	if(_Normals)
		m_MeshData.has_normals = true;
}

GenericMeshLoader::GenericMeshLoader(Pxf::Kernel* _Kernel)
	: ResourceLoader(_Kernel, "Generic Mesh Loader")
{
	Init();
}

bool GenericMeshLoader::Init()
{
	m_Context = ctmNewContext(CTM_IMPORT);

	if(ctmGetError(m_Context) == CTM_NONE)	
	{
		Message(LOCAL_MSG,"OpenCTM Mesh Loader created");
		return true;
	}
	else
	{
		Message(LOCAL_MSG,"Unable to create OpenCTM Context");
		return false;
	}
}

Resource::Mesh* GenericMeshLoader::Load(const char* _FilePath)
{
	ctmLoad(m_Context, _FilePath);
	Resource::Chunk* _Chunk = Resource::LoadFile(_FilePath);   

	// something failed during load
	if(!_Chunk)
	{
		Message(LOCAL_MSG,"Error loading model %s",_FilePath);
		return 0;
	}

	// something is wrong with context
	if(ctmGetError(m_Context) == CTM_NONE)
	{
		// no errors found
		CTMuint _VertCount			= 0;
		CTMuint	_TriCount			= 0;
		const CTMuint*	_Indices	= 0;
		const CTMfloat* _Vertices	= 0;
		const CTMfloat* _Normals	= 0;
		bool _HasNormals			= false;

		_VertCount = ctmGetInteger(m_Context, CTM_VERTEX_COUNT);
		_Vertices = ctmGetFloatArray(m_Context, CTM_VERTICES);
		_TriCount = ctmGetInteger(m_Context, CTM_TRIANGLE_COUNT);
		_Indices = ctmGetIntegerArray(m_Context, CTM_INDICES);

		if(ctmGetInteger(m_Context, CTM_HAS_NORMALS) == CTM_TRUE)
		{
			_HasNormals = true;
			_Normals = ctmGetFloatArray(m_Context,CTM_NORMALS);
		}

		OpenCTMMesh* _NewMesh = new OpenCTMMesh(_Chunk,this);
		_NewMesh->SetData(_VertCount,_TriCount,_Vertices,_Indices,_Normals);
		Message(LOCAL_MSG,"Finished loading model %s", _FilePath);

		return _NewMesh;
	}
	else
	{
		Message(LOCAL_MSG,"Error loading model %s",_FilePath);
		delete _Chunk;
		return 0;
	}
}

Resource::Mesh* GenericMeshLoader::CreateFrom(const void* _DataPtr, unsigned _DataLen)
{
	// LOL DUN NO !!!
	return 0;
}

GenericMeshLoader::~GenericMeshLoader()
{
	ctmFreeContext(m_Context);
}