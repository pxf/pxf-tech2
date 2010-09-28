#ifndef _DERPRENDERER_RENDERER_H_
#define _DERPRENDERER_RENDERER_H_

#include "RenderBlocks.h"

#include <Pxf/Util/String.h>
#include <Pxf/Util/Map.h>

#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Graphics/RenderBuffer.h>
#include <Pxf/Graphics/FrameBufferObject.h>

#include <Pxf/Network/NetworkDevice.h>

#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/Json.h>
#include <Pxf/Resource/Text.h>

#include <json/json.h>
#include <json/writer.h>

namespace Derp
{
  class Renderer
  {
  public:
    Renderer(unsigned int _port);
    virtual ~Renderer ();
	
		void LoadFromFile(const char* _filepath);
		void CleanUp();
    
    void LoadJson();
    void BuildGraph();

		void Execute();

		Pxf::Util::Map<Pxf::Util::String, Block*> m_Blocks;
		
		Pxf::Graphics::Texture* GetResult()
		{
			return m_RootBlock->m_OutputTexture;
		}
		
		int m_Width, m_Height;
		Pxf::Graphics::FrameBufferObject* m_FBO;
		Pxf::Network::Server* m_Net;
		char* m_JsonData;
		unsigned int m_JsonDataSize;
		
		
		int m_NetTag_Pipeline, m_NetTag_Result, m_NetTag_Profiling;

  private:
    //const char* m_Filepath;
		Pxf::Resource::Json* m_doc;
		Pxf::Resource::JsonLoader* m_jsonloader;
		Json::Value root; // json-root
		
		Pxf::Graphics::GraphicsDevice *m_gfx;
		
		// internal pipeline representation
		const char* m_RootName;
		RootBlock* m_RootBlock;
		
  };
} /* Derp */

#endif /* _DERPRENDERER_RENDERER_H_ */