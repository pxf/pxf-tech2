#include <Pxf/Pxf.h>
#include <Pxf/Base/Logger.h>
#include <Pxf/Base/Memory.h>
#include <Pxf/Util/String.h>
#include <Pxf/Modules/pri/DeviceGL2.h>
#include <Pxf/Modules/pri/VertexBufferGL2.h>
#include <Pxf/Modules/pri/TextureGL2.h>
#include <Pxf/Modules/pri/RenderBufferGL2.h>
#include <Pxf/Modules/pri/FrameBufferObjectGL2.h>
#include <Pxf/Modules/pri/ShaderGLSL.h>
#include <Pxf/Modules/pri/ModelGL2.h>
#include <Pxf/Graphics/Font.h>
#include <Pxf/Graphics/PrimitiveBatch.h>
//#include <Pxf/Input/OpenGL/InputGL2.h>
#include <Pxf/Base/Debug.h>

#include <Pxf/Modules/pri/OpenGL.h>
#include <Pxf/Modules/pri/OpenGLUtils.h>
#include <Pxf/Modules/pri/UniGL.h>

#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/Image.h>
#include <Pxf/Resource/Mesh.h>
#include <Pxf/Resource/Font.h>
#include <Pxf/Resource/Text.h>

#include <string.h>
#include <sys/stat.h>

using namespace Pxf;
using namespace Pxf::Graphics;
using namespace Pxf::Modules;
using Util::String;

DeviceGL2::DeviceGL2(Pxf::Kernel* _Kernel)
	: GraphicsDevice(_Kernel, "OpenGL2 Graphics Device")
	, m_CurrentFrameBufferObject(0)
	, m_CurrentShader(0)
	, m_PrimitiveBatch(0)
	, m_Window(0)
	, m_LogTag(0)
{
	m_LogTag = _Kernel->CreateTag("gfx");

	// Initialize GLFW
	if (glfwInit() != GL_TRUE)
	{
		GetKernel()->Log(m_LogTag,"Could not initialize GLFW!");
		return;
	}
	
	// Clear BindTexture history
	for(int i = 0; i < 16; ++i)
		m_BindHistory[i] = NULL;
}

DeviceGL2::~DeviceGL2()
{
	if (m_PrimitiveBatch)
		delete m_PrimitiveBatch;

	// Close any open window.
	CloseWindow();
	

	// Terminate GLFW
	glfwTerminate();
}

Window* DeviceGL2::OpenWindow(WindowSpecifications* _pWindowSpecs)
{
	m_Window = new WindowGL2(this, _pWindowSpecs);
	if (m_Window->Open())
		return m_Window;
	else
		return NULL;
}

Window* DeviceGL2::GetWindow()
{
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

// TODO: Set color (using \1, \2, \3? DefineColor(1, {255, 0, 0}))
float DeviceGL2::Print(Graphics::Font* _Font, float _X, float _Y, float _Scale, const char* _Text)
{
	// Create primitive batch if it doesn't exist.
	if (!m_PrimitiveBatch)
	{
		m_PrimitiveBatch = new PrimitiveBatch(this);
	}

	if (!_Font->IsReady())
		return 0.f;

	const Resource::Font::CharInfo_t* info;
	const Resource::Font* font = _Font->GetFont();

	// Fetch window resolution
	float ResW = m_Window->GetWidth();
	float ResH = m_Window->GetHeight();

	int Len = strlen(_Text);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, ResW, ResH, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glPushAttrib(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	BindTexture((Graphics::Texture*)_Font->GetTexture());
	glPushAttrib(GL_BLEND);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_PrimitiveBatch->QuadsBegin();
	m_PrimitiveBatch->SetColor(0.f, 0.f, 0.f, 1.f);
	glTranslatef(0.375f, 0.375f, 0.f);
	glScalef(_Scale, _Scale, 0.f);

	// round and scale
	float x = ceilf(_X) / _Scale;
	float y = ceilf(_Y) / _Scale;

	float width = 0;

	char last_char = 0;
	for (int i = 0; i < Len; i++)
	{
		if (_Text[i] == '\n')
		{
			y += ceilf(_Font->LineHeight()) * _Scale;
			width = 0;
		}
		else if (_Text[i] == '\t')
		{
			info = font->GetInfo(' ');
			width += ceilf(2*info->Width * _Scale);
		}
		else if (!(_Text[i] >= 0 && _Text[i] < 32))
		{
			info = font->GetInfo(_Text[i]);
			int kerning = font->GetKernings(last_char, _Text[i]);
			m_PrimitiveBatch->QuadsSetTextureSubset(info->Tx1, info->Ty1, info->Tx2, info->Ty2);
			m_PrimitiveBatch->QuadsDrawTopLeft(x + width + info->XOffset + kerning, y + info->YOffset, info->Width, info->Height);
			width += info->XAdvance + kerning;
		}
	}

	m_PrimitiveBatch->QuadsEnd();
	BindTexture(0);

	glPopAttrib(); // blending
	glPopAttrib(); // depth testing


	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW); // set the right matrix mode...

	return width*_Scale;
}

void DeviceGL2::SetDepthFunction(DepthFuncType _DepthType)
{
	int _DepthFunc;

	switch(_DepthType)
	{
		case DF_NEVER:		_DepthFunc = GL_NEVER; break;
		case DF_ALWAYS:		_DepthFunc = GL_ALWAYS; break;
		case DF_LESS:		_DepthFunc = GL_LESS; break;
		case DF_EQUAL:		_DepthFunc = GL_EQUAL; break;
		case DF_LEQUAL:		_DepthFunc = GL_LEQUAL; break;
		case DF_GREATER:	_DepthFunc = GL_GREATER; break;
		case DF_NOTEQUAL:	_DepthFunc = GL_NOTEQUAL; break;
		case DF_GEQUAL:		_DepthFunc = GL_GEQUAL; break;
		default:			_DepthFunc = GL_LESS; break;
	}

	glDepthFunc(_DepthFunc);
}

void DeviceGL2::SetDepthState(bool _State)
{
	if(_State)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
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
	PXFGLCHECK("DeviceGL2::BindTexture/Start");
	Texture* ret = m_BindHistory[0];
	m_BindHistory[0] = _texture;
	glActiveTextureARB(_texture_units_array[0]);
	if (_texture == NULL)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
	} else {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, ((TextureGL2*)_texture)->GetTextureID());
	}
	PXFGLCHECK("DeviceGL2::BindTexture/End");
	return ret;
}


Texture* DeviceGL2::BindTexture(Texture* _texture, unsigned int _texture_unit)
{
	PXFGLCHECK("DeviceGL2::BindTexture2/Start");
	Texture* ret = m_BindHistory[_texture_unit];
	m_BindHistory[_texture_unit] = _texture;
	glActiveTextureARB(_texture_units_array[_texture_unit]);
	if (_texture == NULL)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	} else {
		glBindTexture(GL_TEXTURE_2D, ((TextureGL2*)_texture)->GetTextureID());
	}
	PXFGLCHECK("DeviceGL2::BindTexture2/End");
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
	PXFGLCHECK("DeviceGL2::CreateTextureFromFramebuffer/Start");
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
	PXFGLCHECK("DeviceGL2::CreateTextureFromFramebuffer/End");
	return tex;
}


Resource::Image* DeviceGL2::CreateImageFromTexture(Graphics::Texture* _texture)
{
	PXFGLCHECK("DeviceGL2::CreateImageFromTexture/Start");
	Resource::ImageLoader* ldr = GetKernel()->GetResourceManager()->FindResourceLoader<Resource::ImageLoader>("png");
	int width = _texture->GetWidth();
	int height = _texture->GetHeight();
	int channels = 4;
	unsigned char* pixels = (unsigned char*)MemoryAllocate(width*height*channels);
	
	Texture* old_tex = BindTexture(_texture);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	BindTexture(old_tex);
	
	Resource::Image* img = ldr->CreateFromRaw(width, height, channels, pixels);
	MemoryFree(pixels);
	PXFGLCHECK("DeviceGL2::CreateImageFromTexture/End");
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
	PXFGLCHECK("DeviceGL2::DrawBuffer/Start");
	PXF_ASSERT(_VertexCount <= _pVertexBuffer->GetVertexCount(), "Attempting to draw too many vertices");

	if (m_Window->IsMinimized())
		return;

	_pVertexBuffer->_PreDraw();
	GLuint primitive = LookupPrimitiveType(_pVertexBuffer->GetPrimitive());
	unsigned vertex_count = _pVertexBuffer->GetVertexCount();
	if (_VertexCount > 0)
		vertex_count = _VertexCount;
	glDrawArrays(primitive, 0, vertex_count);
	_pVertexBuffer->_PostDraw();
	PXFGLCHECK("DeviceGL2::DrawBuffer/End");
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
	PXFGLCHECK("DeviceGL2::BindFrameBufferObject/Start");
	// prepare drawing

	if(_pFrameBufferObject)
	{
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		Graphics::FrameBufferObject* _OldFBO = m_CurrentFrameBufferObject;
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, ((FrameBufferObjectGL2*) _pFrameBufferObject)->GetHandle());
		m_CurrentFrameBufferObject = _pFrameBufferObject;
		
		int _buffercount = 0;
		GLenum _attachment_lut[] = {GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT,
		                GL_COLOR_ATTACHMENT3_EXT, GL_COLOR_ATTACHMENT4_EXT, GL_COLOR_ATTACHMENT5_EXT,
		                GL_COLOR_ATTACHMENT6_EXT, GL_COLOR_ATTACHMENT7_EXT, GL_COLOR_ATTACHMENT8_EXT};

		_buffercount = ((FrameBufferObjectGL2*) _pFrameBufferObject)->GetNumAttached();
		if (_buffercount > 1)
			glDrawBuffers(_buffercount, _attachment_lut);
		else
			glDrawBuffer(_attachment_lut[0]);

		PXFGLCHECK("DeviceGL2::BindFrameBufferObject/End");
		return _OldFBO;
	}
	else
	{
		Graphics::FrameBufferObject* _OldFBO = m_CurrentFrameBufferObject;
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		m_CurrentFrameBufferObject = 0;
		glDrawBuffer(GL_BACK);

		PXFGLCHECK("DeviceGL2::BindFrameBufferObject/End");
		return _OldFBO;
	}
}

void DeviceGL2::UnbindFrameBufferObject()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	m_CurrentFrameBufferObject = 0;
	glDrawBuffer(GL_BACK);
}


Shader* DeviceGL2::CreateShader(const char* _Ident, const char* _VertexShader, const char* _FragmentShader)
{
	if (_Ident && _VertexShader && _FragmentShader)
	{
		return new ShaderGLSL(this, _Ident, _VertexShader, _FragmentShader);
	}
	return NULL;
}

Graphics::Shader* DeviceGL2::CreateShaderFromPath(const char* _Ident, const char* _VertexShaderPath, const char* _FragmentShaderPath)
{
	if(!_Ident || !_VertexShaderPath || !_FragmentShaderPath)
		return 0;

	Resource::ResourceManager* res = GetKernel()->GetResourceManager();
	Resource::Text* _VSData = res->Acquire<Resource::Text>(_VertexShaderPath);
	Resource::Text* _FSData = res->Acquire<Resource::Text>(_FragmentShaderPath);

	if(!_VSData || !_FSData)
		return 0;

	ShaderGLSL *_Shader = new ShaderGLSL(this,_Ident,_VSData->Ptr(),_FSData->Ptr());
	
	_Shader->m_FragmentPath = _FragmentShaderPath;
	_Shader->m_VertexPath = _VertexShaderPath;

	struct stat _Info; 
	if(!stat(_FragmentShaderPath,&_Info))
		_Shader->m_FragmentLastUpdated = _Info.st_mtime;
	if(!stat(_VertexShaderPath,&_Info))
		_Shader->m_VertexLastUpdated = _Info.st_mtime;

	return _Shader;
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
