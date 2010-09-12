#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Modules/json/JsonLoader.h>

#define LOCAL_MSG "JsonLoader"

using namespace Pxf;
using namespace Modules;

/*
	JsonCpp
*/

bool JsonCpp::Build()
{
	bool success = m_Reader.parse((char*)m_Chunk->data, m_Root);

	if (!success)
	{
		Message(LOCAL_MSG, "Failed to parse json file.");
		Message(LOCAL_MSG, "%s", m_Reader.getFormatedErrorMessages().c_str());
	}

	return success;
}

JsonCpp::~JsonCpp()
{
}

/*
	JsonCppLoader
*/

JsonCppLoader::JsonCppLoader(Pxf::Kernel* _Kernel)
	: JsonLoader(_Kernel, "Json Loader")
{
	Init();
}

bool JsonCppLoader::Init()
{
	return false;
}

Resource::Json* JsonCppLoader::Load(const char* _FilePath)
{
	Resource::Chunk* chunk = Resource::LoadFile(_FilePath);                   
	if (!chunk)
	{
		Message("JsonLoader", "Unable to create chunk from file '%s'", _FilePath);
		return NULL;
	}
	return new JsonCpp(m_Kernel, chunk, this);
}

Resource::Json* JsonCppLoader::CreateFrom(const void* _DataPtr, unsigned _DataLen)
{
	Resource::Chunk* chunk = new Resource::Chunk();
	chunk->data = (void*) _DataPtr;
	chunk->size = _DataLen;
	chunk->is_static = true;
	return new JsonCpp(m_Kernel, chunk, this);
}

JsonCppLoader::~JsonCppLoader()
{
}