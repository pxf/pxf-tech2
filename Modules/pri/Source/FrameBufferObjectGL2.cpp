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
}

void FrameBufferObjectGL2::AddColorAttachment(Graphics::RenderBuffer* _Attachment)
{}
void FrameBufferObjectGL2::AddDepthAttachment(Graphics::RenderBuffer* _Depth)
{}

void FrameBufferObjectGL2::DetachColor(unsigned _ID)
{}
void FrameBufferObjectGL2::DetachDepth(unsigned _ID)
{}

