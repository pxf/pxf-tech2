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
	return NULL;
}

Resource::Json* JsonLoader::CreateFrom(const void* _DataPtr, unsigned _DataLen)
{

	return 0;
}

JsonLoader::~JsonLoader()
{
}