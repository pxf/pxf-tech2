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
	m_Reader = new ::Json::Reader();
	m_Root = new ::Json::Value;
	bool success = m_Reader->parse((char*)m_Chunk->data, *m_Root);
	return success;
	/* TODO: Make a resource loader for json */
	/*
	::Json::Value root;
	::Json::Reader reader;
	bool success = reader.parse("{\"honk\": 42}", root);
	if (success)
		Message("json", "%d", root.get("honk", 88).asInt());
	return true;
	*/
}

JsonCpp::~JsonCpp()
{
	if (m_Root)
		delete m_Root;
	if (m_Reader)
		delete m_Reader;
}

Resource::Json::Value* JsonCpp::Get(const char* _String, const char* _DefaultValue)
{
	::Json::Value val = m_Root->get(_String, _DefaultValue);
	return new JsonCppValue();
}

Resource::Json::Value* JsonCpp::Get(const char* _String, int _DefaultValue)
{
	::Json::Value val = m_Root->get(_String, _DefaultValue);
	return new JsonCppValue();
}
Resource::Json::Value* JsonCpp::Get(const char* _String, bool _DefaultValue)
{
	::Json::Value val = m_Root->get(_String, _DefaultValue);
	return new JsonCppValue();
}

Resource::Json::Value* JsonCpp::Get(int _Value, int _DefaultValue)
{
	::Json::Value val = m_Root->get(_Value, _DefaultValue);
	return new JsonCppValue();
}
Resource::Json::Value* JsonCpp::Get(bool _Value, bool _DefaultValue)
{
	::Json::Value val = m_Root->get(_Value, _DefaultValue);
	return new JsonCppValue();
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
	return false;
}

Util::String JsonCpp::JsonCppValue::asString()
{
	return "";
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