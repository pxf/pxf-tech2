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

PFNGLDRAWBUFFERSPROC GL::DrawBuffers = 0;

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
PFNGLGETUNIFORMLOCATIONPROC GL::GetUniformLocation = 0;

// Uniform (float, float vectors)
PFNGLUNIFORM1FPROC GL::Uniform1f = 0;
PFNGLUNIFORM2FPROC GL::Uniform2f = 0;
PFNGLUNIFORM3FPROC GL::Uniform3f = 0;
PFNGLUNIFORM4FPROC GL::Uniform4f = 0;
PFNGLUNIFORM1FVPROC GL::Uniform1fv = 0;
PFNGLUNIFORM2FVPROC GL::Uniform2fv = 0;
PFNGLUNIFORM3FVPROC GL::Uniform3fv = 0;
PFNGLUNIFORM4FVPROC GL::Uniform4fv = 0;

// Unifom (integer, integer vectors)
PFNGLUNIFORM1IPROC GL::Uniform1i = 0;
PFNGLUNIFORM2IPROC GL::Uniform2i = 0;
PFNGLUNIFORM3IPROC GL::Uniform3i = 0;
PFNGLUNIFORM4IPROC GL::Uniform4i = 0;
PFNGLUNIFORM1IVPROC GL::Uniform1iv = 0;
PFNGLUNIFORM2IVPROC GL::Uniform2iv = 0;
PFNGLUNIFORM3IVPROC GL::Uniform3iv = 0;
PFNGLUNIFORM4IVPROC GL::Uniform4iv = 0;

// Uniform (matrices)
PFNGLUNIFORMMATRIX2FVPROC GL::UniformMatrix2fv = 0;
PFNGLUNIFORMMATRIX3FVPROC GL::UniformMatrix3fv = 0;
PFNGLUNIFORMMATRIX4FVPROC GL::UniformMatrix4fv = 0;

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

	/* GL_ARB_draw_buffers */
	if (glewIsSupported("ARB_draw_buffers"))
	{
		DrawBuffers = glDrawBuffersARB;
	}

	/* OpenGL 2.0 */
	if (GLEW_VERSION_2_0)
	{
		COMPILE_STATUS = GL_COMPILE_STATUS;
		LINK_STATUS = GL_LINK_STATUS;
		VERTEX_SHADER = GL_VERTEX_SHADER;
		FRAGMENT_SHADER = GL_FRAGMENT_SHADER;

		DrawBuffers = glDrawBuffers;

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
		GetUniformLocation = glGetUniformLocation;

		Uniform1f = glUniform1f;
		Uniform2f = glUniform2f;
		Uniform3f = glUniform3f;
		Uniform4f = glUniform4f;
		Uniform1fv = glUniform1fv;
		Uniform2fv = glUniform2fv;
		Uniform3fv = glUniform3fv;
		Uniform4fv = glUniform4fv;

		Uniform1i = glUniform1i;
		Uniform2i = glUniform2i;
		Uniform3i = glUniform3i;
		Uniform4i = glUniform4i;
		Uniform1iv = glUniform1iv;
		Uniform2iv = glUniform2iv;
		Uniform3iv = glUniform3iv;
		Uniform4iv = glUniform4iv;

		UniformMatrix2fv = glUniformMatrix2fv;
		UniformMatrix3fv = glUniformMatrix3fv;
		UniformMatrix4fv = glUniformMatrix4fv;
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
		GetUniformLocation = glGetUniformLocationARB;

		Uniform1f = glUniform1fARB;
		Uniform2f = glUniform2fARB;
		Uniform3f = glUniform3fARB;
		Uniform4f = glUniform4fARB;
		Uniform1fv = glUniform1fvARB;
		Uniform2fv = glUniform2fvARB;
		Uniform3fv = glUniform3fvARB;
		Uniform4fv = glUniform4fvARB;

		Uniform1i = glUniform1iARB;
		Uniform2i = glUniform2iARB;
		Uniform3i = glUniform3iARB;
		Uniform4i = glUniform4iARB;
		Uniform1iv = glUniform1ivARB;
		Uniform2iv = glUniform2ivARB;
		Uniform3iv = glUniform3ivARB;
		Uniform4iv = glUniform4ivARB;

		UniformMatrix2fv = glUniformMatrix2fvARB;
		UniformMatrix3fv = glUniformMatrix3fvARB;
		UniformMatrix4fv = glUniformMatrix4fvARB;
	}
}