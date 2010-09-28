#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Stream.h>
#include <Pxf/Modules/json/JsonLoader.h>

#include <Pxf/Base/Logger.h>
#include <Pxf/Kernel.h>

#define LOCAL_MSG "JsonLoader"

#include <string>
#include <json/writer.h>

using namespace Pxf;
using namespace Modules;

/*
	JsonCpp
*/

JsonCpp::JsonCpp(Kernel* _Kernel, Resource::Chunk* _Chunk, Resource::ResourceLoader* _Loader)
	: Resource::Json(_Kernel, _Chunk,_Loader)
{
	m_LogTag = m_Kernel->CreateTag("res");
	Build();
}

bool JsonCpp::Build()
{
	// empty document
	if (!m_Chunk)
		return true;

	bool success = m_Reader.parse((char*)m_Chunk->data, m_Root);

	if (!success)
	{
		m_Kernel->Log(m_LogTag | Logger::IS_CRITICAL, "Failed to parse json file.");
		m_Kernel->Log(m_LogTag | Logger::IS_CRITICAL, "%s", m_Reader.getFormatedErrorMessages().c_str());
	}

	return success;
}

JsonCpp::~JsonCpp()
{
}

bool JsonCpp::SaveToDisk(const char* _FilePath)
{
	::Json::StyledWriter writer;
	std::string json_text = writer.write(m_Root).c_str();
	FileStream fs;
	if (!fs.OpenWriteBinary(_FilePath))
		return false;
	fs.Write(json_text.c_str(), json_text.size());
	fs.Close();
	return true;
}

/*
	JsonCppLoader
*/

JsonCppLoader::JsonCppLoader(Pxf::Kernel* _Kernel)
	: JsonLoader(_Kernel, "Json Loader")
	, m_LogTag(0)
{
	m_LogTag = m_Kernel->CreateTag("res");
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
		m_Kernel->Log(m_LogTag | Logger::IS_CRITICAL, "Unable to create chunk from file '%s'", _FilePath);
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

Resource::Json* JsonCppLoader::CreateEmpty()
{
	return new JsonCpp(m_Kernel, 0, this);
}

JsonCppLoader::~JsonCppLoader()
{
}