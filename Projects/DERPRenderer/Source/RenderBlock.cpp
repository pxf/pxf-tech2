#include "RenderBlocks.h"
#include "Renderer.h"

#include <Pxf/Kernel.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Debug.h>

#include <Pxf/Graphics/Texture.h>
#include <Pxf/Graphics/GraphicsDevice.h>

#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/Json.h>

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

bool AuxiliaryBlock::Initialize(Json::Value *node)
{
	//Json::Value node = CreateJson(m_JsonData);
	
	// First set blockname
	m_BlockName = (*node)["blockName"].asCString();
	
	// Take care of what type of aux block this is, and set AuxData
	if ((*node)["blockData"]["auxType"].asString() == "texture")
	{
		m_AuxType = AUXILIARY_TEXTURE;
		m_AuxData = (*node)["blockData"]["filepath"].asCString();
	}
	else if ((*node)["blockData"]["auxType"].asString() == "script")
	{
		m_AuxType = AUXILIARY_SCRIPT;
		m_AuxData = (*node)["blockData"]["script"].asCString();
	}
	
	// Add outputs
	for(int i = 0; i < (*node)["blockOutput"].size(); i++)
	{
		m_Outputs.insert( std::make_pair((*node)["blockOutput"][i]["name"].asCString(), (*node)["blockOutput"][i]["type"].asCString()) );
	}
	
	return true;
}

void AuxiliaryBlock::BuildGraph()
{
	// I have no inputs just do nothing!
	//Message("AUXBLOCK", "My name is: %s", m_BlockName);
	
	if (!m_HasBeenBuilt)
	{
		m_TextureOutput = Pxf::Kernel::GetInstance()->GetGraphicsDevice()->CreateTexture(m_AuxData);
		
		m_HasBeenBuilt = true;
	}
}

bool RenderBlock::Initialize(Json::Value *node)
{
	return false;
}

bool PostProcessBlock::Initialize(Json::Value *node)
{
	return false;
}

bool RootBlock::Initialize(Json::Value *node)
{
	
	// First set blockname
	m_BlockName = (*node)["blockName"].asCString();
	
	// Setup texture properties
	m_Width = (*node)["blockData"]["width"].asInt();
	m_Height = (*node)["blockData"]["height"].asInt();
	
	// Add inputs
	for(int i = 0; i < (*node)["blockInput"].size(); i++)
	{
		m_Inputs.insert( std::make_pair((*node)["blockInput"][i]["block"].asCString(), (*node)["blockInput"][i]["output"].asCString()) );
	}
	
	// TODO: Add other root properties
	
	return true;
}

void RootBlock::BuildGraph()
{
	if (!m_HasBeenBuilt)
	{
		// Build childs in trees
		for (Util::Map<Util::String, Util::String>::iterator iter = m_Inputs.begin(); iter != m_Inputs.end(); ++iter)
		{
			Block *child = m_Renderer->m_Blocks[(*iter).first.c_str()];
			child->BuildGraph();
			m_InputBlocks.insert( std::make_pair((*iter).first.c_str(), child) );
		}	
	
		// Build up input pointers
		/*for (Util::Map<Util::String, Util::String>::iterator iter = m_Inputs.begin(); iter != m_Inputs.end(); ++iter)
		{
			Message("LOOOOOOOOOOOOOOOOOOOOOL", (*iter).first.c_str());
			//m_Inputs.insert( std::make_pair(node["blockInput"][i]["block"].asCString(), node["blockInput"][i]["output"].asCString()) );
		}*/
		
		m_HasBeenBuilt = true;
	}
}

bool RootBlock::Execute()
{
	m_OutputTexture = ((AuxiliaryBlock*)(m_InputBlocks["auxinput1"]))->m_TextureOutput;
	return Block::Execute();
}

