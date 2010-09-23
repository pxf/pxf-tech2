#include <Pxf/Pxf.h>
#include <Pxf/Base/Memory.h>
#include <Pxf/Util/String.h>
#include <Pxf/Modules/pri/DeviceGL2.h>
#include <Pxf/Modules/pri/VertexBufferGL2.h>
#include <Pxf/Modules/pri/TextureGL2.h>
#include <Pxf/Modules/pri/RenderBufferGL2.h>
#include <Pxf/Modules/pri/FrameBufferObjectGL2.h>
#include <Pxf/Modules/pri/ShaderGLSL.h>
#include <Pxf/Modules/pri/ModelGL2.h>
//#include <Pxf/Input/OpenGL/InputGL2.h>
#include <Pxf/Base/Debug.h>

#include <Pxf/Modules/pri/OpenGL.h>
#include <Pxf/Modules/pri/OpenGLUtils.h>
#include <Pxf/Modules/pri/UniGL.h>

#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/Image.h>
#include <Pxf/Resource/Mesh.h>


#define LOCAL_MSG "Device"

using namespace Pxf;
using namespace Pxf::Graphics;
using namespace Pxf::Modules;
using Util::String;

DeviceGL2::DeviceGL2(Pxf::Kernel* _Kernel)
	: GraphicsDevice(_Kernel, "OpenGL2 Graphics Device")
	, m_CurrentFrameBufferObject(0)
	, m_CurrentShader(0)
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

void DeviceGL2::SetModelView(Math::Mat4 *_matrix)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((GLfloat*)(_matrix->m));
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

Texture* DeviceGL2::CreateTexture(Resource::Image* _Image)
{
	_Image->_AddRef();
	TextureGL2* _tex = new TextureGL2(this);
	_tex->LoadData(_Image->Ptr(), _Image->Width(), _Image->Height(), _Image->Channels());
	return _tex;	
}

Texture* DeviceGL2::CreateTextureFromData(const unsigned char* _datachunk, int _width, int _height, int _channels)
{
	TextureGL2* _tex = new TextureGL2(this);
	_tex->LoadData(_datachunk, _width, _height, _channels);
	return _tex;
}

static GLuint _texture_units_array[16] = {GL_TEXTURE0_ARB, GL_TEXTURE1_ARB, GL_TEXTURE2_ARB, GL_TEXTURE3_ARB, GL_TEXTURE4_ARB,
										GL_TEXTURE5_ARB, GL_TEXTURE6_ARB, GL_TEXTURE7_ARB, GL_TEXTURE8_ARB, GL_TEXTURE9_ARB,
										GL_TEXTURE10_ARB, GL_TEXTURE11_ARB, GL_TEXTURE12_ARB, GL_TEXTURE13_ARB, GL_TEXTURE14_ARB,
										GL_TEXTURE15_ARB};
Texture* DeviceGL2::BindTexture(Texture* _texture)
{
	Texture* ret = m_BindHistory[0];
	m_BindHistory[0] = _texture;
	glActiveTextureARB(_texture_units_array[0]);
	if (_texture == NULL)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	} else {
		glBindTexture(GL_TEXTURE_2D, ((TextureGL2*)_texture)->GetTextureID());
	}
	return ret;
}


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

void DeviceGL2::DestroyTexture(Graphics::Texture* _texture)
{
  if (_texture)
	delete _texture;
}

static void flip_image(int x, int y, int w, int h, int c, unsigned char* pixeldata)
{
	/* invert image */
	for(int j = 0; j*2 < h; ++j)
	{
		int index1 = j * w * c;
		int index2 = (h - 1 - j) * w * c;
		for(int i = w * c; i > 0; --i)
		{
			unsigned char temp = pixeldata[index1];
			pixeldata[index1] = pixeldata[index2];
			pixeldata[index2] = temp;
			++index1;
			++index2;
		}
	}
}

Graphics::Texture* DeviceGL2::CreateTextureFromFramebuffer()
{
	int x = 0;
	int y = 0;
	int w = m_Window->GetWidth();
	int h = m_Window->GetHeight();
	int c = 3;

	unsigned char* pixeldata = (unsigned char*)MemoryAllocate(w * h * 3);
	glReadPixels (x, y, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixeldata);
	flip_image(x, y, w, h, c, pixeldata);
	Texture* tex = CreateTextureFromData(pixeldata, w, h, c);
	MemoryFree(pixeldata);
	return tex;
}


Resource::Image* DeviceGL2::CreateImageFromTexture(Graphics::Texture* _texture)
{
	Resource::ImageLoader* ldr = GetKernel()->GetResourceManager()->FindResourceLoader<Resource::ImageLoader>("png");
	int width = _texture->GetWidth();
	int height = _texture->GetHeight();
	int channels = 3;
	unsigned char* pixels = (unsigned char*)MemoryAllocate(width*height*channels);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	Resource::Image* img = ldr->CreateFromRaw(width, height, channels, pixels);
	MemoryFree(pixels);
	return img;
}

Model* DeviceGL2::CreateModel(const char* _FilePath)
{
	ModelGL2* _NewModel = new ModelGL2(this);
	_NewModel->Load(_FilePath);
	return _NewModel;
}

Model* DeviceGL2::CreateModel(Resource::Mesh* _Mesh)
{
	_Mesh->_AddRef();
	ModelGL2* _NewModel = new ModelGL2(this);
	_NewModel->Load((Resource::Mesh*)_Mesh);
	return _NewModel;	
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
	// prepare drawing
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	if(_pFrameBufferObject)
	{
		Graphics::FrameBufferObject* _OldFBO = m_CurrentFrameBufferObject;
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, ((FrameBufferObjectGL2*) _pFrameBufferObject)->GetHandle());
		m_CurrentFrameBufferObject = _pFrameBufferObject;
		
		int _buffercount = 0;
		GLenum _attachment_lut[] = {GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT,
		                GL_COLOR_ATTACHMENT3_EXT, GL_COLOR_ATTACHMENT4_EXT, GL_COLOR_ATTACHMENT5_EXT,
		                GL_COLOR_ATTACHMENT6_EXT, GL_COLOR_ATTACHMENT7_EXT, GL_COLOR_ATTACHMENT8_EXT};

		_buffercount = ((FrameBufferObjectGL2*) _pFrameBufferObject)->GetNumAttached();
		Message("DeviceGL2:::BindFrameBufferObject", "buffercount: %i", _buffercount);
		if (_buffercount > 1)
			glDrawBuffers(_buffercount, _attachment_lut);
		else
			glDrawBuffer(_attachment_lut[0]);

		return _OldFBO;
	}
	else
	{
		Graphics::FrameBufferObject* _OldFBO = m_CurrentFrameBufferObject;
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		m_CurrentFrameBufferObject = 0;
		glDrawBuffer(GL_BACK);

		return _OldFBO;
	}
}

void DeviceGL2::UnbindFrameBufferObject()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	m_CurrentFrameBufferObject = 0;
	//glDrawBuffer(GL_BACK);
}


Shader* DeviceGL2::CreateShader(const char* _Ident, const char* _VertexShader, const char* _FragmentShader)
{
	if (_Ident && _VertexShader && _FragmentShader)
	{
		return new ShaderGLSL(this, _Ident, _VertexShader, _FragmentShader);
	}
	return NULL;
}

void DeviceGL2::DestroyShader(Shader* _Shader)
{
	if (_Shader)
		delete _Shader;
}

Shader* DeviceGL2::BindShader(Shader* _Shader)
{
	Shader *previous = m_CurrentShader;
	if (_Shader)
	{
		m_CurrentShader = _Shader;
		GL::UseProgram(((ShaderGLSL*)_Shader)->GetProgramHandle());
	}
	else 
	{
		m_CurrentShader = NULL;
		GL::UseProgram(0);
	}

	return previous;
}

void DeviceGL2::SetUniformi(Graphics::Shader* _Shader, const char* _name, int _value)
{
	Shader* old_shader = m_CurrentShader;
	BindShader(_Shader);
	int loc = GL::GetUniformLocation(((ShaderGLSL*)_Shader)->GetProgramHandle(), _name);
	GL::Uniform1i(loc, _value);
	BindShader(old_shader); 
}

void DeviceGL2::SetUniformf(Graphics::Shader* _Shader, const char* _name, float _value)
{
	Shader* old_shader = m_CurrentShader;
	BindShader(_Shader);
	int loc = GL::GetUniformLocation(((ShaderGLSL*)_Shader)->GetProgramHandle(), _name);
	GL::Uniform1f(loc, _value);
	BindShader(old_shader); 
}

void DeviceGL2::SetUniformVec2(Graphics::Shader* _Shader, const char* _name, const Math::Vec2f* _value)
{
	Shader* old_shader = m_CurrentShader;
	BindShader(_Shader);
	int loc = GL::GetUniformLocation(((ShaderGLSL*)_Shader)->GetProgramHandle(), _name);
	GL::Uniform2fv(loc, 1, (const float*)_value);
	BindShader(old_shader); 
}

void DeviceGL2::SetUniformVec3(Graphics::Shader* _Shader, const char* _name, const Math::Vec3f* _value)
{
	Shader* old_shader = m_CurrentShader;
	BindShader(_Shader);
	int loc = GL::GetUniformLocation(((ShaderGLSL*)_Shader)->GetProgramHandle(), _name);
	GL::Uniform3fv(loc, 1, (const float*)_value);
	BindShader(old_shader);
}

void DeviceGL2::SetUniformVec3v(Graphics::Shader* _Shader, const char* _name, unsigned count, const Math::Vec3f* _value)
{
	Shader* old_shader = m_CurrentShader;
	BindShader(_Shader);
	int loc = GL::GetUniformLocation(((ShaderGLSL*)_Shader)->GetProgramHandle(), _name);
	GL::Uniform3fv(loc, count, (const float*)_value);
	BindShader(old_shader);
}

void DeviceGL2::SetUniformVec4(Graphics::Shader* _Shader, const char* _name, const Math::Vec4f* _value)
{
	Shader* old_shader = m_CurrentShader;
	BindShader(_Shader);
	int loc = GL::GetUniformLocation(((ShaderGLSL*)_Shader)->GetProgramHandle(), _name);
	GL::Uniform4fv(loc, 1, (const float*)_value);
	BindShader(old_shader);
}

void DeviceGL2::SetUniformVec4v(Graphics::Shader* _Shader, const char* _name, unsigned count, const Math::Vec4f* _value)
{
	Shader* old_shader = m_CurrentShader;
	BindShader(_Shader);
	int loc = GL::GetUniformLocation(((ShaderGLSL*)_Shader)->GetProgramHandle(), _name);
	GL::Uniform4fv(loc, count, (const float*)_value);
	BindShader(old_shader);
}

void DeviceGL2::SetUniformMat4(Graphics::Shader* _Shader, const char* _name, const Math::Mat4* _value)
{
	Shader* old_shader = m_CurrentShader;
	BindShader(_Shader);
	int loc = GL::GetUniformLocation(((ShaderGLSL*)_Shader)->GetProgramHandle(), _name);
	GL::UniformMatrix4fv(loc, 1, 0, _value->m);
	BindShader(old_shader);
}