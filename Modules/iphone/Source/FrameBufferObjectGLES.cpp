#include <Pxf/Modules/iphone/FrameBufferObjectGLES.h>
#include <Pxf/Modules/iphone/RenderBufferGLES.h>
#include <Pxf/Modules/iphone/TextureGLES.h>
#include <Pxf/Modules/iphone/UniGL.h>
#include <Pxf/Base/Logger.h>
#include <Pxf/Kernel.h>

#include <stdio.h>

#define LOCAL_MSG "FrameBufferObject"

using namespace Pxf;
using namespace Pxf::Graphics;
using namespace Pxf::Modules;

unsigned TranslateAttachment(unsigned _ID);
bool CheckFBO(GLenum _status);

FrameBufferObjectGLES::FrameBufferObjectGLES(Graphics::GraphicsDevice* _Device)
	: FrameBufferObject(_Device)
	, m_LogTag(0)
{
	m_LogTag = m_pDevice->GetKernel()->CreateTag("gfx");
	_Configure();
}

FrameBufferObjectGLES::~FrameBufferObjectGLES()
{
	DetachAll();

	glDeleteFramebuffersEXT(1, &m_Handle);
}

void FrameBufferObjectGLES::DetachAll()
{
}

void FrameBufferObjectGLES::_Configure()
{
	PXFGLCHECK("_Configure/Start");
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
	PXFGLCHECK("_Configure/End");
}

bool FrameBufferObjectGLES::CheckFBO(unsigned _status)
{
	switch(_status)
	{
	case GL_FRAMEBUFFER_COMPLETE_EXT:
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

// Lut for id -> opengl 
unsigned TranslateAttachment(unsigned _ID)
{
	switch(_ID)
	{
	case GL_COLOR_ATTACHMENT0_EXT: return 0; break;
	case GL_COLOR_ATTACHMENT1_EXT: return 1; break;
	case GL_COLOR_ATTACHMENT2_EXT: return 2; break;
	case GL_COLOR_ATTACHMENT3_EXT: return 3; break;
	case GL_COLOR_ATTACHMENT4_EXT: return 4; break;
	case GL_COLOR_ATTACHMENT5_EXT: return 5; break;
	case GL_COLOR_ATTACHMENT6_EXT: return 6; break;
	case GL_COLOR_ATTACHMENT7_EXT: return 7; break;
	case GL_COLOR_ATTACHMENT8_EXT: return 8; break;
	case GL_COLOR_ATTACHMENT9_EXT: return 9; break;
	case GL_COLOR_ATTACHMENT10_EXT: return 10; break;
	case GL_COLOR_ATTACHMENT11_EXT: return 11; break;
	case GL_COLOR_ATTACHMENT12_EXT: return 12; break;
	case GL_COLOR_ATTACHMENT13_EXT: return 13; break;
	case GL_COLOR_ATTACHMENT14_EXT: return 14; break;
	case GL_COLOR_ATTACHMENT15_EXT: return 15; break;
	default: break;
	}

	return 0;
}

void FrameBufferObjectGLES::Detach(const unsigned _Attachment)
{
	PXFGLCHECK("Detach/Start");
	if(_Attachment == GL_DEPTH_ATTACHMENT_EXT)
	{
		if(!m_UseDepthAttachment)
		{
			Message(LOCAL_MSG,"Unable to detach depth buffer, no such buffer attached");
			return;
		}

		m_UseDepthAttachment = false;
	}
	else if(_Attachment == GL_STENCIL_ATTACHMENT_EXT)
	{
		if(!m_UseStencilAttachment)
		{
			Message(LOCAL_MSG,"Unable to detach stencil buffer, no such buffer attached");
			return;
		}

		m_UseStencilAttachment = false;
	}
	else if(_Attachment >= GL_COLOR_ATTACHMENT0 && _Attachment <= GL_COLOR_ATTACHMENT15)
	{
		short unsigned _ID = TranslateAttachment(_Attachment);
		unsigned short _Result = m_AttachmentMask & (1 << (_ID));

		if(!_Result)
		{
			// attachment is not attached
			Message(LOCAL_MSG,"Unable to detach unattached attachment");
			return;
		}	

		m_NumColorAttachment--;
		m_AttachmentMask &= ~(1 << (_ID));

	}
	else
	{
		Message(LOCAL_MSG,"Unable to detach an invalid attachment");
		return;
	}

	FrameBufferObject* OldFBO = m_pDevice->BindFrameBufferObject(this);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, _Attachment, GL_TEXTURE_2D, 0, 0);
	m_pDevice->BindFrameBufferObject(OldFBO);
	PXFGLCHECK("Detach/End");
}

void FrameBufferObjectGLES::Attach(Texture* _Texture, const unsigned _Attachment, bool _GenMipmaps)
{
	PXFGLCHECK("Attach Tex/End");
	if(!_Texture)
	{
		Message(LOCAL_MSG,"Unable to attach with invalid texture");
		return;
	}

	if(_Attachment == GL_DEPTH_ATTACHMENT_EXT) 
	{
		if(m_UseDepthAttachment)
			Detach(_Attachment);

		m_UseDepthAttachment = true;
	}

	else if(_Attachment == GL_STENCIL_ATTACHMENT_EXT)
	{
		if(m_UseStencilAttachment)
			Detach(_Attachment);

		m_UseStencilAttachment = true;
	}

	else if(_Attachment >= GL_COLOR_ATTACHMENT0 && _Attachment <= GL_COLOR_ATTACHMENT15)
	{
		short unsigned _ID = TranslateAttachment(_Attachment);
		short unsigned _Result = (m_AttachmentMask & (1 << (_ID)));

		if(_Result)
		{
			// already attached, detach first!
			Detach(_Attachment);
			Message(LOCAL_MSG,"Already attached, reattaching with new ID");
		}	
		
		
		m_NumColorAttachment++;
		m_AttachmentMask |= (1 << (_ID));
	}
	else
	{
		Message(LOCAL_MSG,"Unable to attach with invalid ID");
		return;
	}

	// everything OK, attach it
	FrameBufferObject* OldFBO = m_pDevice->BindFrameBufferObject(this);

	Texture* OldTex = m_pDevice->BindTexture(_Texture);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, _Attachment, GL_TEXTURE_2D, ((TextureGLES*) _Texture)->GetTextureID(), 0);

	if (_GenMipmaps)
		glGenerateMipmapEXT(GL_TEXTURE_2D);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

	m_pDevice->BindTexture(OldTex);
	m_pDevice->BindFrameBufferObject(OldFBO);
	m_Complete = CheckFBO(status);
	PXFGLCHECK("Attach Tex/End");
}

void FrameBufferObjectGLES::Attach(RenderBuffer* _Buffer, const unsigned _Attachment)
{
	PXFGLCHECK("Attach RB/Start");
	if(!_Buffer)
	{
		Message(LOCAL_MSG,"Invalid renderbuffer passed to attach function");
		return;
	}

	if(!_Buffer->Ready())
	{
		Message(LOCAL_MSG,"Unable to add attachment, attachment is not ready");
		return;
	}

	if(_Attachment == GL_DEPTH_ATTACHMENT_EXT) 
	{
		if(m_UseDepthAttachment)
		{
			Detach(_Attachment);
			Message(LOCAL_MSG,"Depth buffer already attached, reattaching with new ID");
		}

		m_UseDepthAttachment = true;
	}

	else if(_Attachment == GL_STENCIL_ATTACHMENT_EXT)
	{
		if(m_UseStencilAttachment)
		{
			Detach(_Attachment);
			Message(LOCAL_MSG,"Stencil buffer already attached, reattaching with new ID");
		}

		m_UseStencilAttachment = true;
	}

	else if(_Attachment >= GL_COLOR_ATTACHMENT0 && _Attachment <= GL_COLOR_ATTACHMENT15)
	{
		short unsigned _ID = TranslateAttachment(_Attachment);
		short unsigned _Result = (m_AttachmentMask & (1 << (_ID)));

		if(_Result)
		{
			// already attached, detach first!
			Detach(_Attachment);
			Message(LOCAL_MSG,"Already attached, reattaching with new ID");
		}	
		
		m_NumColorAttachment++;
		m_AttachmentMask |= 1 << (_ID);
	}
	else
	{
		Message(LOCAL_MSG,"Unable to attach with invalid ID");
		return;
	}

	FrameBufferObject* OldFBO = m_pDevice->BindFrameBufferObject(this);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, _Attachment,
							 GL_RENDERBUFFER_EXT, ((RenderBufferGLES*) _Buffer)->GetHandle());
	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	m_pDevice->BindFrameBufferObject(OldFBO);
	m_Complete = CheckFBO(status);

	PXFGLCHECK("Attach RB/End");
}

int FrameBufferObjectGLES::GetNumAttached()
{
	return m_NumColorAttachment;
}

