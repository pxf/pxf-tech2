#include <Pxf/Modules/pri/TypeTraits.h>
#include <Pxf/Base/Debug.h>

using namespace Pxf;
using namespace Pxf::Graphics;

// Constants
unsigned int GL::ARRAY_BUFFER = 0;

// Buffer
PFNGLDELETEBUFFERSPROC GL::DeleteBuffers = 0;
PFNGLBINDBUFFERPROC GL::BindBuffer = 0;
PFNGLGENBUFFERSPROC GL::GenBuffers = 0;
PFNGLBUFFERDATAPROC GL::BufferData = 0;
PFNGLBUFFERSUBDATAPROC GL::BufferSubData = 0;
PFNGLMAPBUFFERPROC GL::MapBuffer = 0;
PFNGLUNMAPBUFFERPROC GL::UnmapBuffer = 0;

// Shader
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
	}
	/* ARB support for shaders */
	else if (glewIsSupported("ARB_vertex_program ARB_fragment_program"))
	{
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
	}
}