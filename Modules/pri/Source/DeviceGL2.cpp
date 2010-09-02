#include <Pxf/Pxf.h>
#include <Pxf/Util/String.h>
#include <Pxf/Modules/pri/DeviceGL2.h>
#include <Pxf/Modules/pri/VertexBufferGL2.h>
#include <Pxf/Modules/pri/TextureGL2.h>
#include <Pxf/Modules/pri/RenderBufferGL2.h>
#include <Pxf/Modules/pri/FrameBufferObjectGL2.h>
//#include <Pxf/Input/OpenGL/InputGL2.h>
#include <Pxf/Base/Debug.h>

#include <Pxf/Modules/pri/OpenGL.h>
#include <Pxf/Modules/pri/OpenGLUtils.h>


#define LOCAL_MSG "Device"

using namespace Pxf;
using namespace Pxf::Graphics;
using namespace Pxf::Modules;
using Util::String;

DeviceGL2::DeviceGL2(Pxf::Kernel* _Kernel)
    : GraphicsDevice(_Kernel, "OpenGL2 Graphics Device")
	, m_CurrentFrameBufferObject(0)
{
	// Initialize GLFW
	if (glfwInit() != GL_TRUE)
	{
		Message(LOCAL_MSG, "Could not initialize GLFW!");
		return;
	}

	Message(LOCAL_MSG, "Device initiated.");
	
	// Clear BindTexture history
	for(int i = 0; i < 16; ++i)
        m_BindHistory[i] = NULL;
	
}

DeviceGL2::~DeviceGL2()
{

	// Close any open window.
	CloseWindow();
	

	// Terminate GLFW
	glfwTerminate();
}

Window* DeviceGL2::OpenWindow(WindowSpecifications* _pWindowSpecs)
{
	m_Window = new WindowGL2(_pWindowSpecs);
	m_Window->Open();
	return m_Window;
}

void DeviceGL2::CloseWindow()
{
	if (m_Window && m_Window->IsOpen())
	{
		m_Window->Close();
		delete m_Window;
	}
}

void DeviceGL2::SetViewport(int _x, int _y, int _w, int _h)
{
	glViewport(_x, _y, _w, _h);
}

void DeviceGL2::SetProjection(Math::Mat4 *_matrix)
{
	glMatrixMode (GL_PROJECTION);
	glLoadMatrixf((GLfloat*)(_matrix->m));
	glMatrixMode (GL_MODELVIEW);
}

void DeviceGL2::SwapBuffers()
{
	if (m_Window)
	{
		m_Window->Swap();
	}
}

void DeviceGL2::Translate(Math::Vec3f _translate)
{
	glTranslatef(_translate.x, _translate.y, _translate.z);
}

Texture* DeviceGL2::CreateEmptyTexture(int _Width,int _Height, TextureFormatStorage _Format)
{
	TextureGL2* _Tex = new TextureGL2(this);
	_Tex->LoadData(NULL,_Width,_Height,_Format);

	return _Tex;
}

Texture* DeviceGL2::CreateTexture(const char* _filepath)
{
	TextureGL2* _tex = new TextureGL2(this);
	_tex->Load(_filepath);
	return _tex;
}

Texture* DeviceGL2::CreateTextureFromData(const unsigned char* _datachunk, int _width, int _height, int _channels)
{
	TextureGL2* _tex = new TextureGL2(this);
	_tex->LoadData(_datachunk, _width, _height, _channels);
	return _tex;
}

Texture* DeviceGL2::BindTexture(Texture* _texture)
{
    Texture* ret = m_BindHistory[0];
    m_BindHistory[0] = _texture;
    if (_texture == NULL)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    } else {
        glBindTexture(GL_TEXTURE_2D, ((TextureGL2*)_texture)->GetTextureID());
    }
    return ret;
}

static GLuint _texture_units_array[16] = {GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3, GL_TEXTURE4,
										GL_TEXTURE5, GL_TEXTURE6, GL_TEXTURE7, GL_TEXTURE8, GL_TEXTURE9,
										GL_TEXTURE10, GL_TEXTURE11, GL_TEXTURE12, GL_TEXTURE13, GL_TEXTURE14,
										GL_TEXTURE15};
Texture* DeviceGL2::BindTexture(Texture* _texture, unsigned int _texture_unit)
{
    Texture* ret = m_BindHistory[_texture_unit];
    m_BindHistory[_texture_unit] = _texture;
	glActiveTextureARB(_texture_units_array[_texture_unit]);
    if (_texture == NULL)
    {
    	glBindTexture(GL_TEXTURE_2D, 0);
    } else {
    	glBindTexture(GL_TEXTURE_2D, ((TextureGL2*)_texture)->GetTextureID());
    }
    return ret;
}

VertexBuffer* DeviceGL2::CreateVertexBuffer(VertexBufferLocation _VertexBufferLocation, VertexBufferUsageFlag _VertexBufferUsageFlag)
{
	return new VertexBufferGL2(this, _VertexBufferLocation, _VertexBufferUsageFlag);
}

void DeviceGL2::DestroyVertexBuffer(VertexBuffer* _pVertexBuffer)
{
	if (_pVertexBuffer)
		delete _pVertexBuffer;
}

static unsigned LookupPrimitiveType(VertexBufferPrimitiveType _PrimitiveType)
{
	switch(_PrimitiveType)
	{
	case VB_PRIMITIVE_POINTS:		return GL_POINTS;
	case VB_PRIMITIVE_LINES:		return GL_LINES;
	case VB_PRIMITIVE_LINE_LOOP:	return GL_LINE_LOOP;
	case VB_PRIMITIVE_LINE_STRIP:	return GL_LINE_STRIP;
	case VB_PRIMITIVE_TRIANGLES:	return GL_TRIANGLES;
	case VB_PRIMITIVE_TRIANGLE_STRIP:	return GL_TRIANGLE_STRIP;
	case VB_PRIMITIVE_TRIANGLE_FAN:	return GL_TRIANGLE_FAN;
	case VB_PRIMITIVE_QUADS:		return GL_QUADS;
	case VB_PRIMITIVE_QUAD_STRIP:	return GL_QUAD_STRIP;
	}
	PXF_ASSERT(0, "Unknown primitive type.");
	return 0;
}

void DeviceGL2::DrawBuffer(VertexBuffer* _pVertexBuffer, unsigned _VertexCount)
{
    PXF_ASSERT(_VertexCount <= _pVertexBuffer->GetVertexCount(), "Attempting to draw too many vertices");
    _pVertexBuffer->_PreDraw();
    GLuint primitive = LookupPrimitiveType(_pVertexBuffer->GetPrimitive());
    unsigned vertex_count = _pVertexBuffer->GetVertexCount();
    if (_VertexCount > 0)
        vertex_count = _VertexCount;
    glDrawArrays(primitive, 0, vertex_count);
    _pVertexBuffer->_PostDraw();
}

RenderBuffer* DeviceGL2::CreateRenderBuffer(unsigned _Format, unsigned _Width, unsigned _Height)
{
	RenderBufferGL2* pBuf = new RenderBufferGL2(this,_Format,_Width,_Height);
	return pBuf;
}

FrameBufferObject* DeviceGL2::CreateFrameBufferObject()
{
	FrameBufferObjectGL2* pBuf = new FrameBufferObjectGL2(this);
	return pBuf;
}

void DeviceGL2::DestroyRenderBuffer(RenderBuffer* _pRenderBuffer)
{
	unsigned _Handle = ((RenderBufferGL2*) _pRenderBuffer)->GetHandle();
	glDeleteRenderbuffersEXT(1, &_Handle);
	if(_pRenderBuffer)
		delete _pRenderBuffer;
}

void DeviceGL2::BindRenderBuffer(RenderBuffer* _pRenderBuffer)
{
	if(_pRenderBuffer)
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, ((RenderBufferGL2*) _pRenderBuffer)->GetHandle());
}

void DeviceGL2::UnbindRenderBuffer()
{
	// ehm .. aoeu
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
}

void DeviceGL2::DestroyFrameBufferObject(FrameBufferObject* _pFrameBufferObject)
{
	unsigned _Handle = ((FrameBufferObjectGL2*) _pFrameBufferObject)->GetHandle();
	glDeleteFramebuffersEXT(1, &_Handle);
	if(_pFrameBufferObject)
		delete _pFrameBufferObject;
}

Graphics::FrameBufferObject* DeviceGL2::BindFrameBufferObject(FrameBufferObject* _pFrameBufferObject)
{
	if(_pFrameBufferObject)
	{
		Graphics::FrameBufferObject* _OldFBO = m_CurrentFrameBufferObject;
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, ((FrameBufferObjectGL2*) _pFrameBufferObject)->GetHandle());
		m_CurrentFrameBufferObject = _pFrameBufferObject;

		return _OldFBO;
	}
	else
	{
		Graphics::FrameBufferObject* _OldFBO = m_CurrentFrameBufferObject;
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		m_CurrentFrameBufferObject = 0;

		return _OldFBO;
	}
}

void DeviceGL2::UnbindFrameBufferObject()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	m_CurrentFrameBufferObject = 0;
}