#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Modules/json/JsonLoader.h>

#include <json/json.h>

#define LOCAL_MSG "JsonLoader"

using namespace Pxf;
using namespace Modules;

/*
	JsonCpp
*/
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

Resource::Json::Value* JsonCpp::Get(const char* _String, const char* _DefaultValue)
{
	return 0;
}

Resource::Json::Value* JsonCpp::Get(const Util::String _String, const Util::String _DefaultValue)
{
	return 0;
}
Resource::Json::Value* JsonCpp::Get(int _Value, int _DefaultValue)
{
	return 0;
}
Resource::Json::Value* JsonCpp::Get(bool _Value, bool _DefaultValue)
{
	return 0;
}

/*
	JsonCppValue
*/

int JsonCpp::JsonCppValue::asInt()
{
	return 0;
}
bool JsonCpp::JsonCppValue::asBool()
{
	return 0;
}

Util::String JsonCpp::JsonCppValue::asString()
{
	return 0;
}

Util::Array<Resource::Json::Value*> JsonCpp::JsonCppValue::asArray()
{
	Util::Array<Resource::Json::Value*> ret;
	return ret;
}

Util::Map<Resource::Json::Value*, Resource::Json::Value*> JsonCpp::JsonCppValue::asMap()
{
	Util::Map<Resource::Json::Value*, Resource::Json::Value*> ret;
	return ret;
}

/*
	JsonLoader
*/

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