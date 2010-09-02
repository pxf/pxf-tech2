#include <Pxf/Modules/pri/RenderBufferGL2.h>
#include <Pxf/Modules/pri/OpenGL.h>

#define LOCAL_MSG "RenderBuffer"

using namespace Pxf;
using namespace Pxf::Graphics;
using namespace Pxf::Modules;

void RenderBufferGL2::_InitBuffer()
{
	glGenRenderbuffersEXT(1, &m_Handle);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_Handle);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, m_Format, m_Width, m_Height);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
}

void RenderBufferGL2::ReleaseBuffer()
{

}