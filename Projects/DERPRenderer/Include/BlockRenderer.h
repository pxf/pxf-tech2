#ifndef _PXF_DERPRENDERER_BLOCKRENDERER_H_
#define _PXF_DERPRENDERER_BLOCKRENDERER_H_

namespace Derp
{
	enum BlockType
	{
		BLOCK_TYPE_AUXILIARY,
		BLOCK_TYPE_RENDER,
		BLOCK_TYPE_POSTPROCESS
	};

	enum AuxiliaryType
	{
		AUXILIARY_TEXTURE,
		AUXILIARY_SCRIPT,
		AUXILIARY_CONST
	};

	enum AuxiliaryOutputType
	{
		AUXILIARY_OUTPUT_TEXTURE,
		AUXILIARY_OUTPUT_INT,
		AUXILIARY_OUTPUT_BOOL,
		AUXILIARY_OUTPUT_FLOAT,
		AUXILIARY_OUTPUT_VEC2,
		AUXILIARY_OUTPUT_VEC3,
		AUXILIARY_OUTPUT_MAT2,
		AUXILIARY_OUTPUT_MAT3,
		AUXILIARY_OUTPUT_MAT4
	};

	class Block
	{
	protected:
		BlockType m_BlockType;
	public:
		Block(BlockType _BlockType)
			: m_BlockType(_BlockType)
		{}

		virtual ~Block() {};

		BlockType GetType()
		{
			return m_BlockType;
		}

	};

	
	class AuxiliaryBlock : public Block
	{

	};

	class RenderBlock : public Block
	{

	};

	class PostProcessBlock : public Block
	{

	};
};

#endif