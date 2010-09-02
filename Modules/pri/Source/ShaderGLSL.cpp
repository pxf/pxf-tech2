#include <Pxf/Modules/pri/ShaderGLSL.h>
#include <Pxf/Modules/pri/OpenGL.h>

#include <Pxf/Base/Debug.h>

using namespace Pxf;
using namespace Pxf::Graphics;

ShaderGLSL::ShaderGLSL(GraphicsDevice* _pDevice, const char* _Identifier, const char* _VertexShader, const char* _FragmentShader)
: Shader(_pDevice)
{
	if (_VertexShader && _FragmentShader)
	{
		m_Valid = true;
		m_ProgramHandle = glCreateProgram();

		// Create vertex shader
		m_VertexShaderHandle = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(m_VertexShaderHandle, 1, (const char**)&_VertexShader, NULL);
		glCompileShader(m_VertexShaderHandle);
		CheckForCompilationErrors(m_VertexShaderHandle);

		// Create fragment shader
		m_FragmentShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(m_FragmentShaderHandle, 1, (const char**)&_FragmentShader, NULL);
		glCompileShader(m_FragmentShaderHandle);
		if (!CheckForCompilationErrors(m_FragmentShaderHandle))
			m_Valid = false;

		// Attach vertex and fragment shader
		glAttachShader(m_ProgramHandle, m_VertexShaderHandle);
		glAttachShader(m_ProgramHandle, m_FragmentShaderHandle);

		glLinkProgram(m_ProgramHandle);
		if (!CheckForLinkerErrors(m_ProgramHandle))
			m_Valid = false;
	}

	if (m_Valid)	
		Message("Shader", "Shader '%s' was compiled and linked successfully", _Identifier);
}


bool ShaderGLSL::CheckForCompilationErrors(unsigned _ShaderHandle)
{
	GLint res;
	glGetShaderiv(_ShaderHandle, GL_COMPILE_STATUS, &res);
	if (res == GL_FALSE)
	{
		char buff[4096];
		GLsizei len = 0;
		glGetShaderInfoLog(_ShaderHandle, 4095, &len, buff);
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
	glGetProgramiv(_ProgramHandle, GL_LINK_STATUS, &res);
	if (res == GL_FALSE)
	{
		char buff[4096];
		GLsizei len = 0;
		glGetProgramInfoLog(_ProgramHandle, 4095, &len, buff);
		buff[len] = 0;

		Message("Shader", "Failed to link shader.");
		Message("Shader", buff);
		if (*buff != 0x0)
			Message("Shader", "Shader link error", buff);
		return false;
	}
	return true;
}