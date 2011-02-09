#ifndef _PXF_MODULES_PRI_SHADERGLSL_H_
#define _PXF_MODULES_PRI_SHADERGLSL_H_
#include <Pxf/Graphics/Shader.h>

#include <time.h>


namespace Pxf
{
	namespace Graphics
	{
		class ShaderGLSL : public Graphics::Shader
		{
		protected:
			unsigned m_LogTag;
			unsigned m_ProgramHandle;
			unsigned m_FragmentShaderHandle;
			unsigned m_VertexShaderHandle;

			bool CheckForCompilationErrors(unsigned _ShaderHandle);
			bool CheckForLinkerErrors(unsigned _ProgramHandle);

		public:
			ShaderGLSL(GraphicsDevice* _pDevice, const char* _Identifier, const char* _VertexShader, const char* _FragmentShader);
			virtual ~ShaderGLSL();

			unsigned GetProgramHandle() const
			{
				return m_ProgramHandle;
			}

			bool Load(const char* _VertexShader,const char* _FragmentShader);
			bool Unload();

			const char* m_FragmentPath;
			const char* m_VertexPath;
			time_t m_FragmentLastUpdated;
			time_t m_VertexLastUpdated;
		};
	}
}

#endif // _PXF_MODULES_PRI_SHADERGLSL_H_