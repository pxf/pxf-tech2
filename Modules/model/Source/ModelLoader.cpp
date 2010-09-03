#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Modules/model/ModelLoader.h>

#define LOCAL_MSG "ModelLoader"

using namespace Pxf;
using namespace Modules;

bool OpenCTMModel::Build()
{

	return true;
}

GenericModelLoader::GenericModelLoader(Pxf::Kernel* _Kernel)
	: ResourceLoader(_Kernel, "Generic Model Loader")
{
	Init();
}

bool GenericModelLoader::Init()
{
	m_Context = ctmNewContext(CTM_IMPORT);

	if(ctmGetError(m_Context) == CTM_NONE)	
	{
		Message(LOCAL_MSG,"OpenCTM Model Loader created");
		return true;
	}
	else
	{
		Message(LOCAL_MSG,"Unable to create OpenCTM Context");
		return false;
	}
}

Resource::Model* GenericModelLoader::Load(const char* _FilePath)
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
		CTMuint			_VertCount, _TriCount;
		const CTMuint*	_Indices;
		const CTMfloat* _Vertices;
		const CTMfloat* _Normals;
		bool _HasNormals = false;

		_VertCount = ctmGetInteger(m_Context, CTM_VERTEX_COUNT);
		_Vertices = ctmGetFloatArray(m_Context, CTM_VERTICES);
		_TriCount = ctmGetInteger(m_Context, CTM_TRIANGLE_COUNT);
		_Indices = ctmGetIntegerArray(m_Context, CTM_INDICES);

		if(ctmGetInteger(m_Context, CTM_HAS_NORMALS) == CTM_TRUE)
		{
			_HasNormals = true;
			_Normals = ctmGetFloatArray(m_Context,CTM_NORMALS);
		}

		OpenCTMModel* _NewModel = new OpenCTMModel(_Chunk,this);
	}
	else
	{
		Message(LOCAL_MSG,"Error loading model %s",_FilePath);
		return 0;
	}
}

Resource::Model* GenericModelLoader::CreateFrom(const void* _DataPtr, unsigned _DataLen)
{
	// LOL DUN NO !!!
	return 0;
}

GenericModelLoader::~GenericModelLoader()
{
	ctmFreeContext(m_Context);
}