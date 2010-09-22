#ifndef _DERPRENDERER_RENDERER_H_
#define _DERPRENDERER_RENDERER_H_

#include "RenderBlocks.h"

#include <Pxf/Util/String.h>
#include <Pxf/Util/Map.h>

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
    Renderer(const char* _filepath);
    virtual ~Renderer ();
    
    void LoadJson();
    void BuildGraph();

		void Execute();

		Pxf::Util::Map<Pxf::Util::String, Block*> m_Blocks;
		
		Pxf::Graphics::Texture* GetResult()
		{
			return m_RootBlock->GetOutputValue();
		}
		
		int m_Width, m_Height;

  private:
    const char* m_Filepath;
		Pxf::Resource::Json* m_doc;
		Pxf::Resource::JsonLoader* m_jsonloader;
		Json::Value root; // json-root
		
		// internal pipeline representation
		const char* m_RootName;
		RootBlock* m_RootBlock;
		
  };
} /* Derp */

#endif /* _DERPRENDERER_RENDERER_H_ */