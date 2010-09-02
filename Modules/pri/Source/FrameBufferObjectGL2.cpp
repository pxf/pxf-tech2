#include <Pxf/Modules/pri/FrameBufferObjectGL2.h>
#include <Pxf/Modules/pri/RenderBufferGL2.h>
#include <Pxf/Modules/pri/OpenGL.h>

#include <stdio.h>

#define LOCAL_MSG "FrameBufferObject"

using namespace Pxf;
using namespace Pxf::Graphics;
using namespace Pxf::Modules;

unsigned ColorAttachmentLookup(unsigned _ID);

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
	GLint _Attachments = 0;
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
	m_pDevice->BindFrameBufferObject(this);
	m_pDevice->BindRenderBuffer(_Attachment);

	unsigned _AttachmentTranslation = ColorAttachmentLookup(_ID);
	unsigned _Handle = ((RenderBufferGL2*) _Attachment)->GetHandle();

	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, _AttachmentTranslation, GL_RENDERBUFFER_EXT, _Handle);

	m_NumColorAttachment++;

	m_pDevice->UnbindFrameBufferObject();
}

// Lut for id -> opengl 
unsigned ColorAttachmentLookup(unsigned _ID)
{
	switch(_ID)
	{
	case 0: return GL_COLOR_ATTACHMENT0_EXT; break;
	case 1: return GL_COLOR_ATTACHMENT1_EXT; break;
	case 2: return GL_COLOR_ATTACHMENT2_EXT; break;
	default: break;
	}
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

