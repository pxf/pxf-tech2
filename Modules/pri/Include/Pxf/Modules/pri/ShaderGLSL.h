#ifndef _PXF_MODULES_PRI_SHADERGLSL_H_
#define _PXF_MODULES_PRI_SHADERGLSL_H_
#include <Pxf/Graphics/Shader.h>

namespace Pxf
{
	namespace Graphics
	{
		class ShaderGLSL : public Graphics::Shader
		{
		protected:
			unsigned m_ProgramHandle;
			unsigned m_FragmentShaderHandle;
			unsigned m_VertexShaderHandle;

			bool CheckForCompilationErrors(unsigned _ShaderHandle);
			bool CheckForLinkerErrors(unsigned _ProgramHandle);
		public:
			ShaderGLSL(GraphicsDevice* _pDevice, const char* _Identifier, const char* _VertexShader, const char* _FragmentShader);
			~ShaderGLSL();

			unsigned GetProgramHandle() const
			{
				return m_ProgramHandle;
			}
		};
	}
}

#endif // _PXF_MODULES_PRI_SHADERGLSL_H_