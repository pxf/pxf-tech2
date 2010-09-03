#include <Pxf/Modules/pri/TypeTraits.h>
#include <Pxf/Base/Debug.h>

using namespace Pxf;
using namespace Pxf::Graphics;

// Buffer
unsigned int GL::ARRAY_BUFFER = 0;
PFNGLDELETEBUFFERSPROC GL::DeleteBuffers = 0;
PFNGLBINDBUFFERPROC GL::BindBuffer = 0;
PFNGLGENBUFFERSPROC GL::GenBuffers = 0;
PFNGLBUFFERDATAPROC GL::BufferData = 0;
PFNGLBUFFERSUBDATAPROC GL::BufferSubData = 0;
PFNGLMAPBUFFERPROC GL::MapBuffer = 0;
PFNGLUNMAPBUFFERPROC GL::UnmapBuffer = 0;

// Shader
unsigned int GL::LINK_STATUS = 0;
unsigned int GL::COMPILE_STATUS = 0;
unsigned int GL::VERTEX_SHADER = 0;
unsigned int GL::FRAGMENT_SHADER = 0;

PFNGLCREATEPROGRAMPROC GL::CreateProgram = 0;
PFNGLCREATESHADERPROC GL::CreateShader = 0;
PFNGLSHADERSOURCEPROC GL::ShaderSource = 0;
PFNGLCOMPILESHADERPROC GL::CompileShader = 0;
PFNGLATTACHSHADERPROC GL::AttachShader = 0;
PFNGLLINKPROGRAMPROC GL::LinkProgram = 0;
PFNGLUSEPROGRAMPROC GL::UseProgram = 0;
PFNGLDETACHSHADERPROC GL::DetachShader = 0;
PFNGLDELETESHADERPROC GL::DeleteShader = 0;
PFNGLDELETEPROGRAMPROC GL::DeleteProgram = 0;
PFNGLGETSHADERIVPROC GL::GetShaderiv = 0;
PFNGLGETPROGRAMIVPROC GL::GetProgramiv = 0;
PFNGLGETSHADERINFOLOGPROC GL::GetShaderInfoLog = 0;
PFNGLGETPROGRAMINFOLOGPROC GL::GetProgramInfoLog = 0;

PFNGLUNIFORM1FPROC GL::Uniform1f;
PFNGLUNIFORM2FPROC GL::Uniform2f;
PFNGLUNIFORM3FPROC GL::Uniform3f;
PFNGLUNIFORM4FPROC GL::Uniform4f;

PFNGLUNIFORM1FVPROC GL::Uniform1fv;
PFNGLUNIFORM2FVPROC GL::Uniform2fv;
PFNGLUNIFORM3FVPROC GL::Uniform3fv;
PFNGLUNIFORM4FVPROC GL::Uniform4fv;

void GL::SetupExtensions()
{
	/* OpenGL 1.5 */
	if (GLEW_VERSION_1_5)
	{
		ARRAY_BUFFER = GL_ARRAY_BUFFER;
		DeleteBuffers = glDeleteBuffers;
		BindBuffer = glBindBuffer;
		GenBuffers = glGenBuffers;
		BufferData = glBufferData;
		BufferSubData = glBufferSubData;
		MapBuffer = glMapBuffer;
		UnmapBuffer = glUnmapBuffer;
	}

	/* OpenGL 1.4 with VBO support as extension */
	else if (glewIsSupported("GL_VERSION_1_4  ARB_vertex_buffer_object") || glBindBufferARB != 0)
	{
		ARRAY_BUFFER = GL_ARRAY_BUFFER_ARB;
		DeleteBuffers = glDeleteBuffersARB;
		BindBuffer = glBindBufferARB;
		GenBuffers = glGenBuffersARB;
		BufferData = glBufferDataARB;
		BufferSubData = glBufferSubDataARB;
		MapBuffer = glMapBufferARB;
		UnmapBuffer = glUnmapBufferARB;
	}

	/* OpenGL 2.0 */
	if (GLEW_VERSION_2_0)
	{
		COMPILE_STATUS = GL_COMPILE_STATUS;
		LINK_STATUS = GL_LINK_STATUS;
		VERTEX_SHADER = GL_VERTEX_SHADER;
		FRAGMENT_SHADER = GL_FRAGMENT_SHADER;

		CreateProgram = glCreateProgram;
		CreateShader = glCreateShader;
		ShaderSource = glShaderSource;
		CompileShader = glCompileShader;
		AttachShader = glAttachShader;
		LinkProgram = glLinkProgram;
		UseProgram = glUseProgram;
		DetachShader = glDetachShader;
		DeleteShader = glDeleteShader;
		DeleteProgram = glDeleteProgram;
		GetShaderiv = glGetShaderiv;
		GetProgramiv = glGetProgramiv;
		GetShaderInfoLog = glGetShaderInfoLog;
		GetProgramInfoLog = glGetProgramInfoLog;
		Uniform1f = glUniform1f;
		Uniform2f = glUniform2f;
		Uniform3f = glUniform3f;
		Uniform4f = glUniform4f;
		Uniform1fv = glUniform1fv;
		Uniform2fv = glUniform2fv;
		Uniform3fv = glUniform3fv;
		Uniform4fv = glUniform4fv;
	}
	/* ARB support for shaders */
	else if (glewIsSupported("ARB_vertex_program ARB_fragment_program") || glCreateProgramObjectARB != 0)
	{
		COMPILE_STATUS = GL_OBJECT_COMPILE_STATUS_ARB;
		LINK_STATUS = GL_OBJECT_LINK_STATUS_ARB;
		VERTEX_SHADER = GL_VERTEX_SHADER_ARB;
		FRAGMENT_SHADER = GL_FRAGMENT_SHADER_ARB;
		
		CreateProgram = glCreateProgramObjectARB;
		CreateShader = glCreateShaderObjectARB;
		ShaderSource = glShaderSourceARB;
		CompileShader = glCompileShaderARB;
		AttachShader = glAttachObjectARB;
		LinkProgram = glLinkProgramARB;
		UseProgram = glUseProgramObjectARB;
		DetachShader = glDetachObjectARB;
		DeleteShader = glDeleteObjectARB;
		DeleteProgram = glDeleteObjectARB;
		GetShaderiv = glGetObjectParameterivARB;
		GetProgramiv = glGetObjectParameterivARB;
		GetShaderInfoLog = glGetInfoLogARB;
		GetProgramInfoLog = glGetInfoLogARB;
		Uniform1f = glUniform1fARB;
		Uniform2f = glUniform2fARB;
		Uniform3f = glUniform3fARB;
		Uniform4f = glUniform4fARB;
		Uniform1fv = glUniform1fvARB;
		Uniform2fv = glUniform2fvARB;
		Uniform3fv = glUniform3fvARB;
		Uniform4fv = glUniform4fvARB;
	}
}