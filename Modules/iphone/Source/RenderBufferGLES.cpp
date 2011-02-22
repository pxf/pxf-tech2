#include <Pxf/Modules/iphone/RenderBufferGLES.h>
#include <Pxf/Modules/iphone/OpenGL.h>
#include <Pxf/Base/Logger.h>
#include <Pxf/Kernel.h>

#define LOCAL_MSG "RenderBuffer"

using namespace Pxf;
using namespace Pxf::Graphics;
using namespace Pxf::Modules;

void RenderBufferGLES::_InitBuffer()
{
	m_LogTag = m_pDevice->GetKernel()->CreateTag("gfx");

	glGenRenderbuffersEXT(1, &m_Handle);
	m_pDevice->BindRenderBuffer(this);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, m_Format, m_Width, m_Height);
	m_pDevice->UnbindRenderBuffer();

	if(m_Handle)
	{
		GetDevice()->GetKernel()->Log(m_LogTag, "Created render buffer (%i,%i)",m_Width,m_Height);
		m_Ready = true;
	}
}

RenderBufferGLES::~RenderBufferGLES()
{
	if (m_Handle)
		ReleaseBuffer();
}

void RenderBufferGLES::ReleaseBuffer()
{
	if(m_Handle)
	{
		glDeleteRenderbuffersEXT(1, &m_Handle);
		m_Handle = 0;
	}
}