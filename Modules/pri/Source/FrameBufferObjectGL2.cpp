#include <Pxf/Modules/pri/FrameBufferObjectGL2.h>
#include <Pxf/Modules/pri/OpenGL.h>

#include <stdio.h>

#define LOCAL_MSG "FrameBufferObject"

using namespace Pxf;
using namespace Pxf::Graphics;
using namespace Pxf::Modules;

FrameBufferObjectGL2::~FrameBufferObjectGL2()
{
	DetachAll();

	glDeleteFramebuffersEXT(1, &m_Handle);
}

void FrameBufferObjectGL2::DetachAll()
{
}

void FrameBufferObjectGL2::_Configure()
{
	int _Attachments = 0;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &_Attachments);

	if(_Attachments == GL_INVALID_ENUM)
	{
		Message(LOCAL_MSG,"Invalid enum");
		_Attachments = 0;
	}

	m_MaxColorAttachments = _Attachments;

	// create FBO handle

	glGenFramebuffersEXT(1, &m_Handle);
}

void FrameBufferObjectGL2::AddColorAttachment(Graphics::RenderBuffer* _Attachment, unsigned _ID)
{
	if(!_Attachment)
	{
		Message(LOCAL_MSG,"Invalid color attachment passed to framebuffer");
		return;
	}

	if(!_Attachment->Ready())
	{
		Message(LOCAL_MSG,"Unable to add color attachment, attachment is not complete");
		return;
	}

	if(m_NumColorAttachment >= m_MaxColorAttachments)
	{
		Message(LOCAL_MSG,"Capacity reached, unable to attach");
		return;
	}

	if(_ID > m_MaxColorAttachments-1)
	{
		Message(LOCAL_MSG,"Unable to add color attachment with ID %i", _ID);
		return;
	}

	// everything OK, attach
	// glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_Handle);
	m_pDevice->BindFrameBuffer(this);

	m_NumColorAttachment++;
}

void FrameBufferObjectGL2::AddDepthAttachment(Graphics::RenderBuffer* _Depth)
{
	if(!_Depth)
	{
		Message(LOCAL_MSG,"Invalid depth attachment passed to framebuffer");
		return;
	}

	if(!_Depth->Ready())
	{
		Message(LOCAL_MSG,"Unable to add depth attachment, attachment is not ready");
		return;
	}

	m_UseDepthAttachment = true;
}

void FrameBufferObjectGL2::DetachColor(unsigned _ID)
{


	m_NumColorAttachment--;
}

void FrameBufferObjectGL2::DetachDepth()
{

	m_UseDepthAttachment = false;
}

