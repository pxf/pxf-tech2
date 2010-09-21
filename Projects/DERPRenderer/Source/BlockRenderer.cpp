#include "BlockRenderer.h"
#include <Pxf/Kernel.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Graphics/Texture.h>
#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/Json.h>
#include <json/json.h>

using namespace Derp;
using namespace Pxf;

static Json::Value CreateJson(const char* data)
{
	Kernel* k = Kernel::GetInstance();
	Resource::ResourceManager* res = k->GetResourceManager();
	Resource::JsonLoader* jsonloader = res->FindResourceLoader<Pxf::Resource::JsonLoader>("json");
	Resource::Json* doc = jsonloader->CreateFrom(data, StringLength(data));
	Json::Value root;
	if (doc)
	{
		root = doc->GetRoot();
	} else {
    Message("JSON", "Taak a daaamp?");
	}
	jsonloader->Destroy(doc);
	return root;
}

bool AuxiliaryBlock::Initialize(const char* _JsonData)
{
	return false;
}

bool RenderBlock::Initialize(const char* _JsonData)
{
	return false;
}

bool PostProcessBlock::Initialize(const char* _JsonData)
{
	return false;
}

bool RootBlock::Initialize(const char* _JsonData)
{
	Json::Value block = CreateJson(_JsonData);
	for(int i = 0; i < block.size(); i++)
		Message("RootBlock", block[i]["blockName"].asCString());
	return true;
}