#include <Pxf/Modules/pri/TypeTraits.h>
#include <Pxf/Base/Debug.h>

using namespace Pxf;
using namespace Pxf::Graphics;

unsigned int GL::ARRAY_BUFFER = 0;
PFNGLDELETEBUFFERSPROC GL::DeleteBuffers = 0;
PFNGLBINDBUFFERPROC GL::BindBuffer = 0;
PFNGLGENBUFFERSPROC GL::GenBuffers = 0;
PFNGLBUFFERDATAPROC GL::BufferData = 0;
PFNGLBUFFERSUBDATAPROC GL::BufferSubData = 0;
PFNGLMAPBUFFERPROC GL::MapBuffer = 0;
PFNGLUNMAPBUFFERPROC GL::UnmapBuffer = 0;

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
}