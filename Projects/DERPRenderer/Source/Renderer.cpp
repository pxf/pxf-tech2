#include "Renderer.h"

#include <Pxf/Kernel.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Debug.h>

#include <Pxf/Graphics/Texture.h>

using namespace Derp;
using namespace Pxf;

Renderer::Renderer(const char* _filepath)
	: m_Filepath(_filepath)
	, m_RootBlock(NULL)
	, m_RootName(NULL)
	, m_Width(0)
	, m_Height(0)
{
  // do something?
}

Renderer::~Renderer()
{
  // clean up!
	m_jsonloader->Destroy(m_doc);
}

void Renderer::LoadJson()
{
  Kernel* k = Kernel::GetInstance();

 	Resource::ResourceManager* res = k->GetResourceManager();
  Resource::Text* data = res->Acquire<Resource::Text>(m_Filepath);
	m_jsonloader = res->FindResourceLoader<Pxf::Resource::JsonLoader>("json");
	m_doc = m_jsonloader->CreateFrom(data->Ptr(), StringLength(data->Ptr()));
	
	if (m_doc)
	{
		root = m_doc->GetRoot();
		
		// Loop through the array of blocks
		for(int i = 0; i < root.size(); i++)
		{
			if(root[i]["blockType"].asString() == "PipelineTree")
			{
				m_RootName = root[i]["blockData"]["root"].asCString();
				
			} else if(root[i]["blockType"].asString() == "AuxComp") {
				/*
				 * Create AuxBlock
				 */
				Json::StyledWriter writer;
				Block* newauxblock = new AuxiliaryBlock(this, writer.write(root[i]).c_str());
				newauxblock->Initialize(&root[i]);
				
				// Add newly created aux block to the block list
				m_Blocks.insert( std::make_pair(root[i]["blockName"].asString(), newauxblock));
				
				
			} else if(root[i]["blockType"].asString() == "Root") {
				/*
				 * Create Root block
				 */
				Json::StyledWriter writer;
				Block* newrootblock = new RootBlock(this, writer.write(root[i]).c_str());
				newrootblock->Initialize(&root[i]);
				
				// Add newly created root block to the block list
				m_Blocks.insert( std::make_pair(root[i]["blockName"].asString(), newrootblock));
				
			} else {
				Message("Renderer", "Unknown block type: %s", root[i]["blockType"].asCString());
			}
		}
		
		// Lookup root node and set result specific data.
		if (m_RootName)
		{
			m_RootBlock = (RootBlock*)m_Blocks[m_RootName];
			m_Width = m_RootBlock->m_Width;
			m_Height = m_RootBlock->m_Height;
		}
		
		
	} else {
    Message("Renderer", "Error while loading JSON data.");
	}
}

void Renderer::BuildGraph()
{
	if (m_RootName)
	{
		m_RootBlock->BuildGraph();
		
	} else {
		Message("Renderer", "Trying to build pipeline graph, but unknown root.");
	}
}

void Renderer::Execute()
{
	if (m_RootBlock)
	{
		m_RootBlock->Execute();
	} else {
		Message("Renderer", "Failed to execute root block since it's NULL.");
	}
	
}
