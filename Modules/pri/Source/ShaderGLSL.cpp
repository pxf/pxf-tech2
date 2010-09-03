#include <Pxf/Modules/pri/ShaderGLSL.h>
#include <Pxf/Modules/pri/OpenGL.h>
#include <Pxf/Modules/pri/TypeTraits.h>

#include <Pxf/Base/Debug.h>

using namespace Pxf;
using namespace Pxf::Graphics;

ShaderGLSL::ShaderGLSL(GraphicsDevice* _pDevice, const char* _Identifier, const char* _VertexShader, const char* _FragmentShader)
: Shader(_pDevice)
{
	if (_VertexShader && _FragmentShader)
	{
		m_Valid = true;
		m_ProgramHandle = GL::CreateProgram();

		// Create vertex shader
		m_VertexShaderHandle = GL::CreateShader(GL_VERTEX_SHADER);
		GL::ShaderSource(m_VertexShaderHandle, 1, (const char**)&_VertexShader, NULL);
		GL::CompileShader(m_VertexShaderHandle);
		CheckForCompilationErrors(m_VertexShaderHandle);

		// Create fragment shader
		m_FragmentShaderHandle = GL::CreateShader(GL_FRAGMENT_SHADER);
		GL::ShaderSource(m_FragmentShaderHandle, 1, (const char**)&_FragmentShader, NULL);
		GL::CompileShader(m_FragmentShaderHandle);
		if (!CheckForCompilationErrors(m_FragmentShaderHandle))
			m_Valid = false;

		// Attach vertex and fragment shader
		GL::AttachShader(m_ProgramHandle, m_VertexShaderHandle);
		GL::AttachShader(m_ProgramHandle, m_FragmentShaderHandle);

		GL::LinkProgram(m_ProgramHandle);
		if (!CheckForLinkerErrors(m_ProgramHandle))
			m_Valid = false;
	}

	if (m_Valid)	
		Message("Shader", "Shader '%s' was compiled and linked successfully", _Identifier);
}

ShaderGLSL::~ShaderGLSL()
{
	// Detach shaders from program
	GL::DetachShader(m_ProgramHandle, m_VertexShaderHandle);
	GL::DetachShader(m_ProgramHandle, m_FragmentShaderHandle);

	// Delete shaders
	GL::DeleteShader(m_VertexShaderHandle);
	GL::DeleteShader(m_FragmentShaderHandle);

	// Delete program
	GL::DeleteProgram(m_ProgramHandle);
}


bool ShaderGLSL::CheckForCompilationErrors(unsigned _ShaderHandle)
{
	GLint res;
	GL::GetShaderiv(_ShaderHandle, GL::COMPILE_STATUS, &res);
	if (res == GL_FALSE)
	{
		char buff[4096];
		GLsizei len = 0;
		GL::GetShaderInfoLog(_ShaderHandle, 4095, &len, buff);
		buff[len] = 0;

		Message("Shader", "Failed to compile shader.");
		Message("Shader", buff);
		Message("Shader", "Shader compilation error", buff);
		//exit(1);
		return false;
	}
	return true;
}

bool ShaderGLSL::CheckForLinkerErrors(unsigned _ProgramHandle)
{
	GLint res;
	GL::GetProgramiv(_ProgramHandle, GL::LINK_STATUS, &res);
	if (res == GL_FALSE)
	{
		char buff[4096];
		GLsizei len = 0;
		GL::GetProgramInfoLog(_ProgramHandle, 4095, &len, buff);
		buff[len] = 0;

		Message("Shader", "Failed to link shader.");
		Message("Shader", buff);
		if (*buff != 0x0)
			Message("Shader", "Shader link error", buff);
		return false;
	}
	return true;
}