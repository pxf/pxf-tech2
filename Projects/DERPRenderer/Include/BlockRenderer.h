#ifndef _PXF_DERPRENDERER_BLOCKRENDERER_H_
#define _PXF_DERPRENDERER_BLOCKRENDERER_H_

namespace Pxf
{
	namespace Graphics { class Texture; }
}

namespace Derp
{
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

		static const int NUM_INPUTS = 8;
		static const int NUM_OUTPUTS = 8;
		void* m_Inputs[NUM_INPUTS];
		void* m_Outputs[NUM_OUTPUTS];

		BlockOutputType m_OutputType;
		
    bool m_IsPerformed;
		
	public:
		Block(BlockType _BlockType)
			: m_BlockType(_BlockType)
      , m_IsPerformed(false)
		{
			// memset m_inputs, m_outputs
		}

		virtual bool Initialize(const char* _JsonData) = 0;

		virtual ~Block() {};

		template <typename OutputT>
		OutputT* GetOutputValue(unsigned int index)
		{
			return (OutputT*)m_Outputs[index];
		}

		BlockType GetType()
		{
			return m_BlockType;
		}

	};

	//
	// Auxiliary block base
	//

	class AuxiliaryBlock : public Block
	{
	public:
		AuxiliaryBlock()
			: Block(BLOCK_TYPE_AUXILIARY)
		{}

		virtual bool Initialize(const char* _JsonData);
	};

	//
	// Render block
	//

	class RenderBlock : public Block
	{
	public:
		RenderBlock()
			: Block(BLOCK_TYPE_RENDER)
		{}

		virtual bool Initialize(const char* _JsonData);

		Pxf::Graphics::Texture* GetOutputValue(unsigned int index)
		{
			return (Pxf::Graphics::Texture*) m_Outputs[index];
		}
	};

	//
	// Post-process block
	//

	class PostProcessBlock : public Block
	{
	public:
		PostProcessBlock()
			: Block(BLOCK_TYPE_POSTPROCESS)
		{}

		virtual bool Initialize(const char* _JsonData);

		Pxf::Graphics::Texture* GetOutputValue(unsigned int index)
		{
			return (Pxf::Graphics::Texture*) m_Outputs[index];
		}
	};
	
	//
	// Root block
	//

	class RootBlock : public Block
	{
	protected:
		Pxf::Graphics::Texture* m_OutputTexture;
	public:
		RootBlock()
			: Block(BLOCK_TYPE_ROOT)
		{}

		virtual bool Initialize(const char* _JsonData);

		Pxf::Graphics::Texture* GetOutputValue()
		{
			return m_OutputTexture;
		}
	};
};

#endif