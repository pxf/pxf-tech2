#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Modules/json/JsonLoader.h>

#include <json/json.h>

#define LOCAL_MSG "JsonLoader"

using namespace Pxf;
using namespace Modules;

bool JsonCpp::Build()
{
	/* TODO: Make a resource loader for json */
	::Json::Value root;
	::Json::Reader reader;
	bool success = reader.parse("{\"honk\": 42}", root);
	if (success)
		Message("json", "%d", root.get("honk", 88).asInt());
	return true;
}

JsonLoader::JsonLoader(Pxf::Kernel* _Kernel)
	: ResourceLoader(_Kernel, "Json Loader")
{
	Init();
}

bool JsonLoader::Init()
{
	return false;
}

Resource::Json* JsonLoader::Load(const char* _FilePath)
{
	Resource::Chunk* chunk = Resource::LoadFile(_FilePath);                   
	if (!chunk)
	{
		Message("JsonLoader", "Unable to create chunk from file '%s'", _FilePath);
		return NULL;
	}
	return new JsonCpp(chunk, this);
}

Resource::Json* JsonLoader::CreateFrom(const void* _DataPtr, unsigned _DataLen)
{
	Resource::Chunk* chunk = new Resource::Chunk();
	chunk->data = (void*) _DataPtr;
	chunk->size = _DataLen;
	chunk->is_static = true;
	return new JsonCpp(chunk, this);
}

JsonLoader::~JsonLoader()
{
}