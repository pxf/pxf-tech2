#include <Pxf/Modules/pri/RenderBufferGL2.h>
#include <Pxf/Modules/pri/OpenGL.h>

#define LOCAL_MSG "RenderBuffer"

using namespace Pxf;
using namespace Pxf::Graphics;
using namespace Pxf::Modules;

void RenderBufferGL2::_InitBuffer()
{
	glGenRenderbuffersEXT(1, &m_Handle);
	m_pDevice->BindRenderBuffer(this);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, m_Format, m_Width, m_Height);
	m_pDevice->UnbindRenderBuffer();

	if(m_Handle)
	{
		Message(LOCAL_MSG,"Created render buffer (%i,%i)",m_Width,m_Height);
		m_Ready = true;
	}
}

void RenderBufferGL2::ReleaseBuffer()
{
	if(m_Handle)
		glDeleteRenderbuffersEXT(1, &m_Handle);
}