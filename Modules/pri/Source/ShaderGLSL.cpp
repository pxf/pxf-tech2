#include <Pxf/Modules/pri/ShaderGLSL.h>
#include <Pxf/Modules/pri/OpenGL.h>
#include <Pxf/Modules/pri/UniGL.h>

#include <Pxf/Base/Debug.h>

#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Kernel.h>
#include <Pxf/Base/Logger.h>

using namespace Pxf;
using namespace Pxf::Graphics;

ShaderGLSL::ShaderGLSL(GraphicsDevice* _pDevice, const char* _Identifier, const char* _VertexShader, const char* _FragmentShader)
	: Shader(_pDevice)
	, m_VertexPath(0)
	, m_FragmentPath(0)
	, m_LogTag(0)
{
	m_LogTag = m_pDevice->GetKernel()->CreateTag("gfx");

	Load(_VertexShader,_FragmentShader);

	if (m_Valid)	
		m_pDevice->GetKernel()->Log(m_LogTag | Logger::IS_INFORMATION, "Shader '%s' was compiled and linked successfully", _Identifier);
}

bool ShaderGLSL::Unload()
{
	if(!m_Valid) return true;

	GL::UseProgram(0);
	GL::DetachShader(m_ProgramHandle,m_VertexShaderHandle);
	GL::DetachShader(m_ProgramHandle,m_FragmentShaderHandle);
	m_Valid = false;

	return true;
}

bool ShaderGLSL::Load(const char* _VertexShader,const char* _FragmentShader)
{
	Unload();

	PXFGLCHECK("ShaderGLSL::ShaderGLSL/Start");
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
	PXFGLCHECK("ShaderGLSL::ShaderGLSL/End");

	return true;
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

		m_pDevice->GetKernel()->Log(m_LogTag | Logger::IS_CRITICAL, "Failed to compile shader.");
		m_pDevice->GetKernel()->Log(m_LogTag | Logger::IS_CRITICAL, buff);
		m_pDevice->GetKernel()->Log(m_LogTag | Logger::IS_CRITICAL, "Shader compilation error", buff);
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

		m_pDevice->GetKernel()->Log(m_LogTag | Logger::IS_CRITICAL, "Failed to link shader.");
		m_pDevice->GetKernel()->Log(m_LogTag | Logger::IS_CRITICAL, buff);
		if (*buff != 0x0)
			m_pDevice->GetKernel()->Log(m_LogTag | Logger::IS_CRITICAL, "Shader link error", buff);
		return false;
	}
	return true;
}