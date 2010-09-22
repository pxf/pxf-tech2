#include "RenderBlocks.h"
#include "Renderer.h"

#include <Pxf/Kernel.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Debug.h>

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
		m_OutputTypes.insert( std::make_pair((*node)["blockOutput"][i]["name"].asCString(), (*node)["blockOutput"][i]["type"].asCString()) );
	}
	
	return true;
}

void AuxiliaryBlock::BuildGraph()
{
	// I have no inputs just do nothing!
	//Message("AUXBLOCK", "My name is: %s", m_BlockName);
	
	if (!m_HasBeenBuilt)
	{
		// TODO: check so this is a texture aux
		for (Util::Map<Util::String, Util::String>::iterator iter = m_OutputTypes.begin(); iter != m_OutputTypes.end(); ++iter)
		{
			Graphics::Texture* toutputtex = Pxf::Kernel::GetInstance()->GetGraphicsDevice()->CreateTexture(m_AuxData);
			Message("AuxBlock", "LOL SUP: %i", toutputtex);
			m_Outputs.insert( std::make_pair((*iter).first, (void*)toutputtex) );
		}
		m_HasBeenBuilt = true;
	}
}

void* AuxiliaryBlock::GetOutput(Pxf::Util::String _outputname, Pxf::Util::String _outputtype)
{
	if (m_OutputTypes[_outputname] == _outputtype)
	{
		return m_Outputs[_outputname];
	}
	
	return 0;
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
		
		// Setup internal block stuff
		m_OutputQuad = new SimpleQuad(0, 0, m_Width, m_Height);
		m_OutputTexture = m_gfx->CreateEmptyTexture(m_Width, m_Height);
		
		m_HasBeenBuilt = true;
	}
}

bool RootBlock::Execute()
{
	// Execute prereqs
	for (Util::Map<Util::String, Util::String>::iterator iter = m_Inputs.begin(); iter != m_Inputs.end(); ++iter)
	{
		// Get pointer to input block
		Block* inputblock = m_InputBlocks[(*iter).first];
		inputblock->Execute();
	}
	
	// Setup OGL context etc
	m_gfx->SetViewport(0, 0, m_Width, m_Height);
	Math::Mat4 prjmat = Math::Mat4::Ortho(0, m_Width, 0, m_Height, -1.0f, 10000.0f);
  m_gfx->SetProjection(&prjmat);
	glDisable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT);
	
	// Attach and bind render texture to FBO
	m_Renderer->m_FBO->Attach(m_OutputTexture, GL_COLOR_ATTACHMENT0_EXT, false);
	m_gfx->BindFrameBufferObject(m_Renderer->m_FBO);
	
	// Gather and bind all our inputs
	int ttexunit = 0;
	for (Util::Map<Util::String, Util::String>::iterator iter = m_Inputs.begin(); iter != m_Inputs.end(); ++iter)
	{
		// Get pointer to input block
		Block* inputblock = m_InputBlocks[(*iter).first];
		
		// Get texture pointer
		Graphics::Texture* inputtex = (Graphics::Texture*)inputblock->GetOutput((*iter).second, "texture");
		if (inputtex == 0) {
			Message("RootBlock", "Got a NULL pointer from input: %s (%s).", (*iter).first.c_str(), (*iter).second.c_str());
			return false;
		}
		
		m_gfx->BindTexture(inputtex, ttexunit);
		ttexunit += 1;
	}
	
	// Bind shaders
	// -------- todo ---------
	
	// Render
	m_OutputQuad->Draw();
	
	// Unbind FBO
	m_gfx->UnbindFrameBufferObject();
	
	// Detach texture
	m_Renderer->m_FBO->Detach(GL_COLOR_ATTACHMENT0_EXT);
	
	// Return
	return Block::Execute();
}

