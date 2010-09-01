#include <Pxf/Modules/pri/FrameBufferObjectGL2.h>

#define LOCAL_MSG "FrameBufferObject"

using namespace Pxf;
using namespace Pxf::Graphics;
using namespace Pxf::Modules;

FrameBufferObjectGL2::~FrameBufferObjectGL2()
{
	_DetachAll();
}

void FrameBufferObjectGL2::_DetachAll()
{
}

void FrameBufferObjectGL2::AddColorAttachment(Graphics::RenderBuffer* _Attachment)
{}
void FrameBufferObjectGL2::AddDepthAttachment(Graphics::RenderBuffer* _Depth)
{}

void FrameBufferObjectGL2::DetachColor(unsigned _ID)
{}
void FrameBufferObjectGL2::DetachDepth(unsigned _ID)
{}

