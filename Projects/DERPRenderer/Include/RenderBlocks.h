#ifndef _PXF_DERPRENDERER_RENDERBLOCKS_H_
#define _PXF_DERPRENDERER_RENDERBLOCKS_H_

#include <Pxf/Kernel.h>
#include <Pxf/Util/String.h>
#include <Pxf/Util/Map.h>

#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Graphics/Texture.h>

#include <Pxf/Modules/pri/OpenGL.h>

#include "SimpleQuad.h"

#include <json/json.h>

namespace Pxf
{
	namespace Graphics { class Texture; }
}

namespace Derp
{
	class Renderer;
	
	enum BlockType
	{
		BLOCK_TYPE_ROOT,
		BLOCK_TYPE_AUXILIARY,
		BLOCK_TYPE_RENDER,
		BLOCK_TYPE_POSTPROCESS
	};

	enum BlockOutputType
	{
		BLOCK_OUTPUT_TEXTURE,
		BLOCK_OUTPUT_INT,
		BLOCK_OUTPUT_BOOL,
		BLOCK_OUTPUT_FLOAT,
		BLOCK_OUTPUT_VEC2,
		BLOCK_OUTPUT_VEC3,
		BLOCK_OUTPUT_MAT2,
		BLOCK_OUTPUT_MAT3,
		BLOCK_OUTPUT_MAT4
	};

	enum AuxiliaryType
	{
		AUXILIARY_TEXTURE,
		AUXILIARY_SCRIPT,
		AUXILIARY_CONST
	};

	class Block
	{
	protected:
		BlockType m_BlockType;
		Renderer* m_Renderer;

		/*static const int NUM_INPUTS = 8;
		static const int NUM_OUTPUTS = 8;
		void* m_Inputs[NUM_INPUTS];
		void* m_Outputs[NUM_OUTPUTS];*/

		//BlockOutputType m_OutputType;
		
		// State stuff
		const char *m_BlockName;
    bool m_IsPerformed;
		bool m_HasBeenBuilt;
		
		// graphics device pointer for easy access 8)
		Pxf::Graphics::GraphicsDevice* m_gfx;
		
		// init usage
		Pxf::Util::Map<Pxf::Util::String, Pxf::Util::String> m_OutputTypes; // <name, type>
		Pxf::Util::Map<Pxf::Util::String, void*> m_Outputs; // <name, outputpointer>
		
	public:
		Block(Renderer* _renderer, BlockType _BlockType)
			: m_IsPerformed(false) // m_BlockType(_BlockType)
      , m_Renderer(_renderer)
			, m_HasBeenBuilt(false)
			, m_BlockName(NULL)
		{
			// memset m_inputs, m_outputs
			m_gfx = Pxf::Kernel::GetInstance()->GetGraphicsDevice();
		}

		virtual bool Initialize(Json::Value *node) = 0;

		virtual ~Block() {};
		
		virtual void BuildGraph() {} ;
		
		virtual bool Execute() { m_IsPerformed = true; return true; };

		/*template <typename OutputT>
		OutputT* GetOutputValue(unsigned int index)
		{
			return (OutputT*)m_Outputs[index];
		}*/
		
		virtual void* GetOutput(Pxf::Util::String _outputname, Pxf::Util::String _outputtype) 
		{
			return 0;
		}

		/*BlockType GetType()
		{
			return m_BlockType;
		}*/

	};

	//
	// Auxiliary block base
	//

	class AuxiliaryBlock : public Block
	{
	private:
		AuxiliaryType m_AuxType;
		const char* m_AuxData;
		const char* m_JsonData;
		
		
	public:
		
		AuxiliaryBlock(Renderer* _renderer, const char* _JsonData)
			: Block(_renderer, BLOCK_TYPE_AUXILIARY)
			, m_JsonData(_JsonData)
		{}

		virtual bool Initialize(Json::Value *node);
		
		virtual void BuildGraph();
		
		virtual void* GetOutput(Pxf::Util::String _outputname, Pxf::Util::String _outputtype);
		
		//Pxf::Graphics::Texture *m_TextureOutput;
	};

	//
	// Render block
	//

	class RenderBlock : public Block
	{
	private:
		const char* m_JsonData;
	public:
		RenderBlock(Renderer* _renderer, const char* _JsonData)
			: Block(_renderer, BLOCK_TYPE_RENDER)
			, m_JsonData(_JsonData)
		{}

		virtual bool Initialize(Json::Value *node);

		/*Pxf::Graphics::Texture* GetOutputValue(unsigned int index)
		{
			return (Pxf::Graphics::Texture*) m_Outputs[index];
		}*/
	};

	//
	// Post-process block
	//

	class PostProcessBlock : public Block
	{
	private:
		const char* m_JsonData;
	public:
		PostProcessBlock(Renderer* _renderer, const char* _JsonData)
			: Block(_renderer, BLOCK_TYPE_POSTPROCESS)
			, m_JsonData(_JsonData)
		{}

		virtual bool Initialize(Json::Value *node);

		/*Pxf::Graphics::Texture* GetOutputValue(unsigned int index)
		{
			return (Pxf::Graphics::Texture*) m_Outputs[index];
		}*/
	};
	
	//
	// Root block
	//

	class RootBlock : public Block
	{
	protected:
		
	private:
		//const char *m_InputBlockName;
		//Block* m_InputBlock;
		
		// init usage
		Pxf::Util::Map<Pxf::Util::String, Pxf::Util::String> m_Inputs;
		
		// build graph usage
		Pxf::Util::Map<Pxf::Util::String, Block*> m_InputBlocks;
		
		const char* m_JsonData;
		
		// Root output renderquad
		SimpleQuad* m_OutputQuad;
		
	public:
		RootBlock(Renderer* _renderer, const char* _JsonData)
			: Block(_renderer, BLOCK_TYPE_ROOT)
			, m_JsonData(_JsonData)
			, m_Width(0)
			, m_Height(0)
		{}
		
		virtual ~RootBlock() { delete m_OutputQuad; }

		virtual void BuildGraph();
		virtual bool Initialize(Json::Value *node);
		virtual bool Execute();

		/*Pxf::Graphics::Texture* GetOutputValue()
		{
			return m_OutputTexture;
		}*/
		
		// Root output i.e. final tree result
		Pxf::Graphics::Texture* m_OutputTexture;
		int m_Width, m_Height;
	};
};

#endif