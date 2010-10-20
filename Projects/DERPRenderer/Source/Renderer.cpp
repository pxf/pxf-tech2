#include "Renderer.h"

#include <Pxf/Kernel.h>
#include <Pxf/Base/Stream.h>
#include <Pxf/Base/Memory.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Logger.h>

#include <Pxf/Graphics/Texture.h>
#include <Pxf/Network/NetworkDevice.h>

#include <Pxf/Resource/Image.h>

#include <RemoteLogWriter.h>

using namespace Derp;
using namespace Pxf;
using namespace Graphics;

Renderer::Renderer(unsigned int _port)
	: m_RootBlock(NULL)
	, m_RootName(NULL)
	, m_Width(0)
	, m_Height(0)
	, m_jsonloader(0)
	, m_doc(0)
	, m_JsonData(0)
{
	// Set tags for network.
	m_NetDevice = Pxf::Kernel::GetInstance()->GetNetworkDevice();
	m_NetTag_Pipeline = m_NetDevice->AddTag("pipeline");
	m_NetTag_Datacache = m_NetDevice->AddTag("datacache");
	m_NetTag_Preview = m_NetDevice->AddTag("preview");
	m_NetTag_Profiling = m_NetDevice->AddTag("profiling");
	
	unsigned netlogtag = m_NetDevice->AddTag("log");
	m_Net = m_NetDevice->CreateServer();
	m_Net->Bind(_port);
	Pxf::Kernel::GetInstance()->RegisterLogger(new RemoteLogWriter(m_NetDevice, m_Net, netlogtag));

	m_LogTag = Pxf::Kernel::GetInstance()->CreateTag("renderer");
	
  // Load default blocks
	LoadFromFile("data/default.json");
}

Renderer::~Renderer()
{		
	CleanUp();
}

void Renderer::CleanUp()
{
	// Delete blocks
	for(Util::Map<Util::String, Block*>::iterator iter = m_Blocks.begin(); iter != m_Blocks.end(); ++iter)
	{
		delete (*iter).second;
	}
	m_Blocks.clear();
	
	
	if(m_doc)
	{
		if (m_jsonloader)
			m_jsonloader->Destroy(m_doc);
	}
	
	if (m_JsonData)
		delete [] m_JsonData;
		
}

void Renderer::LoadFromFile(const char* _filepath)
{
	CleanUp();
	
  Kernel* k = Kernel::GetInstance();
 	Resource::ResourceManager* res = k->GetResourceManager();
  Resource::Text* data = res->Acquire<Resource::Text>(_filepath);
	m_jsonloader = res->FindResourceLoader<Pxf::Resource::JsonLoader>("json");
	//m_doc = m_jsonloader->CreateFrom(data->Ptr(), StringLength(data->Ptr()));	

	m_JsonDataSize = StringLength(data->Ptr());	
	m_JsonData = new char[m_JsonDataSize];
	StringCopy(m_JsonData, data->Ptr(), m_JsonDataSize);
	
	LoadJson();
	BuildGraph();
}

void Renderer::LoadJson()
{
	m_doc = m_jsonloader->CreateFrom(m_JsonData, m_JsonDataSize);
	
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
				
			} else if(root[i]["blockType"].asString() == "Post-Process") {
				/*
				 * Create Post-Process block
				 */
				Json::StyledWriter writer;
				Block* newppblock = new PostProcessBlock(this, writer.write(root[i]).c_str());
				newppblock->Initialize(&root[i]);

				// Add newly created post-process block to the block list
				m_Blocks.insert( std::make_pair(root[i]["blockName"].asString(), newppblock));

			} else if(root[i]["blockType"].asString() == "Render") {
				/*
				 * Create Geometry Rendering block
				 */
				Json::StyledWriter writer;
				Block* newrenderblock = new RenderBlock(this, writer.write(root[i]).c_str());
				newrenderblock->Initialize(&root[i]);

				// Add newly created rendering block to the block list
				m_Blocks.insert( std::make_pair(root[i]["blockName"].asString(), newrenderblock));

			} else {
				Kernel::GetInstance()->Log(m_LogTag | Logger::IS_CRITICAL, "Unknown block type: %s", root[i]["blockType"].asCString());

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
		Kernel::GetInstance()->Log(m_LogTag | Logger::IS_CRITICAL, "Error while loading JSON data.");
	}
}

void Renderer::BuildGraph()
{
	if (m_RootName)
	{
		// Setup internal OGL stuff
		m_gfx = Kernel::GetInstance()->GetGraphicsDevice();
		m_FBO = m_gfx->CreateFrameBufferObject();
		
		// Build graph
		m_RootBlock->BuildGraph();
		
	} else {
		Kernel::GetInstance()->Log(m_LogTag | Logger::IS_CRITICAL, "Trying to build pipeline graph, but unknown root.");
	}
}

void Renderer::Execute()
{
	Network::Packet* packet = m_Net->RecvNonBlocking(0);
	if (packet != NULL)
	{
		// Some sort of data we need for rendering future pipeline
		if (packet->GetTag() == m_NetTag_Datacache)
		{
			unsigned long hash = packet->GetObject<unsigned long>(0);
			const char* filename = packet->GetObject<const char*>(1);
			unsigned long datalen = packet->GetObject<unsigned long>(2);
			const char* data = packet->GetObject<const char*>(4);
			char hashstr[9] = {0};
			Format(hashstr, "%X", hash);
			char location[256];
			Format(location, "datacache/%s_%s", hashstr, filename);
			Kernel::GetInstance()->Log(m_LogTag | Logger::IS_INFORMATION, "Saving data to cache: %s", location);
			FileStream stream;
			stream.OpenWriteBinary(location);
			stream.Write(data, datalen);
			stream.Close();
		}
		// New pipeline to render
		else if (packet->GetTag() == m_NetTag_Pipeline)
		{
			CleanUp();
			
			m_JsonDataSize = packet->GetLength();
			m_JsonData = new char[m_JsonDataSize];
			StringCopy(m_JsonData, packet->GetData(), m_JsonDataSize);
			
			Kernel::GetInstance()->Log(m_LogTag | Logger::IS_INFORMATION, "Got new pipeline data, loading JSON and building graph: %s", m_JsonData);
			
			
			LoadJson();
			BuildGraph();

			if (m_RootBlock)
			{
				m_RootBlock->ResetPerformed();
				m_RootBlock->Execute();

				if (m_Net->NumClients() > 0)
				{
					Resource::Image* img = m_gfx->CreateImageFromTexture(GetResult());
					Network::Packet* imgpacket = m_NetDevice->CreateEmptyPacket("imgdata", m_NetTag_Preview);
					imgpacket->PushInt(img->Width());
					imgpacket->PushInt(img->Height());
					imgpacket->PushInt(img->Channels());
					imgpacket->PushString((const char*)img->Ptr(), img->Height()*img->Width()*img->Channels());

					Kernel::GetInstance()->Log(m_LogTag | Logger::IS_INFORMATION, "Sending final image to client '%d'", packet->GetSender());
					m_Net->SendPacket(packet->GetSender(), imgpacket);

					delete imgpacket;
					delete img;
				}
			}
			else
			{
				Kernel::GetInstance()->Log(m_LogTag | Logger::IS_CRITICAL, "Failed to execute root block since it's NULL.");

			}
			
		}
	}
	
	if (m_RootBlock)
	{
		m_RootBlock->ResetPerformed();
		m_RootBlock->Execute();
	}
	else
	{
		Kernel::GetInstance()->Log(m_LogTag | Logger::IS_CRITICAL, "Failed to execute root block since it's NULL.");
	}
	
}
