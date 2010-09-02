#include <Pxf/Modules/pri/FrameBufferObjectGL2.h>
#include <Pxf/Modules/pri/RenderBufferGL2.h>
#include <Pxf/Modules/pri/TextureGL2.h>
#include <Pxf/Modules/pri/OpenGL.h>

#include <stdio.h>

#define LOCAL_MSG "FrameBufferObject"

using namespace Pxf;
using namespace Pxf::Graphics;
using namespace Pxf::Modules;

unsigned ColorAttachmentLookup(unsigned _ID);
bool CheckFBO(GLenum _status);

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

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	m_Complete = CheckFBO(status);
}

bool CheckFBO(GLenum _status)
{
	switch(_status)
	{
	case GL_FRAMEBUFFER_COMPLETE_EXT:
		Message("FrameBuffer::Attach", "GL_FRAMEBUFFER_COMPLETE_EXT");
		return true;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
		Message("Framebuffer::Attach", "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
		Message("Framebuffer::Attach", "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
		Message("Framebuffer::Attach", "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
		Message("Framebuffer::Attach", "GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
		Message("Framebuffer::Attach", "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
		Message("Framebuffer::Attach", "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT");
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
		Message("Framebuffer::Attach", "GL_FRAMEBUFFER_UNSUPPORTED_EXT");
		break;
	default:
		Message("Framebuffer::Attach", "Unknown error. (0x%X)", _status);
		break;
	}
	return false;
}

void FrameBufferObjectGL2::AddColorAttachment(Texture* _Texture, unsigned _ID,  const bool _GenMipmaps)
{
	if(!_Texture)
	{
		Message(LOCAL_MSG,"Invalid color attachment passed to framebuffer");
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
	FrameBufferObject* _CurrentFBO = m_pDevice->BindFrameBufferObject(this);
	
	unsigned _AttachmentTranslation = ColorAttachmentLookup(_ID);

	//Texture* old_tex = m_pDevice->BindTexture(_Texture);
	m_pDevice->BindTexture(_Texture);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, _AttachmentTranslation, GL_TEXTURE_2D, ((TextureGL2*) _Texture)->GetTextureID(), 0);

	if (_GenMipmaps)
		glGenerateMipmapEXT(GL_TEXTURE_2D);

	m_pDevice->BindFrameBufferObject(_CurrentFBO);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	m_Complete = CheckFBO(status);

	m_NumColorAttachment++;
}

// Lut for id -> opengl 
unsigned ColorAttachmentLookup(unsigned _ID)
{
	switch(_ID)
	{
	case 0: return GL_COLOR_ATTACHMENT0_EXT; break;
	case 1: return GL_COLOR_ATTACHMENT1_EXT; break;
	case 2: return GL_COLOR_ATTACHMENT2_EXT; break;
	case 3: return GL_COLOR_ATTACHMENT3_EXT; break;
	case 4: return GL_COLOR_ATTACHMENT4_EXT; break;
	case 5: return GL_COLOR_ATTACHMENT5_EXT; break;
	case 6: return GL_COLOR_ATTACHMENT6_EXT; break;
	case 7: return GL_COLOR_ATTACHMENT7_EXT; break;
	case 8: return GL_COLOR_ATTACHMENT8_EXT; break;
	case 9: return GL_COLOR_ATTACHMENT9_EXT; break;
	case 10: return GL_COLOR_ATTACHMENT10_EXT; break;
	case 11: return GL_COLOR_ATTACHMENT11_EXT; break;
	case 12: return GL_COLOR_ATTACHMENT12_EXT; break;
	case 13: return GL_COLOR_ATTACHMENT13_EXT; break;
	case 14: return GL_COLOR_ATTACHMENT14_EXT; break;
	case 15: return GL_COLOR_ATTACHMENT15_EXT; break;
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

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	m_Complete = CheckFBO(status);

	m_UseDepthAttachment = true;
}

void FrameBufferObjectGL2::DetachColor(unsigned _ID)
{
	if(_ID > m_MaxColorAttachments-1)
	{
		Message(LOCAL_MSG, "Unable to detach color attachment with ID %u",_ID);
		return;
	}

	unsigned _AttachmentTranslation = ColorAttachmentLookup(_ID);

	FrameBufferObject* _CurrentFBO = m_pDevice->BindFrameBufferObject(this);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, _AttachmentTranslation, GL_TEXTURE_2D, 0, 0);
	m_pDevice->BindFrameBufferObject(_CurrentFBO);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	m_Complete = CheckFBO(status);

	m_NumColorAttachment--;
}

void FrameBufferObjectGL2::DetachDepth()
{
	FrameBufferObject* _CurrentFBO = m_pDevice->BindFrameBufferObject(this);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0);
	m_pDevice->BindFrameBufferObject(_CurrentFBO);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	m_Complete = CheckFBO(status);

	m_UseDepthAttachment = false;
}

void FrameBufferObjectGL2::AttachStencil(Graphics::RenderBuffer* _Stencil)
{
	if(!_Stencil)
	{
		Message(LOCAL_MSG,"Invalid depth attachment passed to framebuffer");
		return;
	}

	if(!_Stencil->Ready())
	{
		Message(LOCAL_MSG,"Unable to add depth attachment, attachment is not ready");
		return;
	}

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	m_Complete = CheckFBO(status);

	m_UseStencilAttachment = true;
}

void FrameBufferObjectGL2::DetachStencil()
{
	FrameBufferObject* _CurrentFBO = m_pDevice->BindFrameBufferObject(this);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0);
	m_pDevice->BindFrameBufferObject(_CurrentFBO);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	m_Complete = CheckFBO(status);

	m_UseStencilAttachment = false;
}

