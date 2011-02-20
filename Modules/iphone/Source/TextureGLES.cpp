#include <Pxf/Pxf.h>
#include <Pxf/Util/String.h>
#include <Pxf/Modules/iphone/TextureGLES.h>
#include <Pxf/Modules/iphone/UniGL.h>
#include <Pxf/Base/Debug.h>

#include <Pxf/Kernel.h>
#include <Pxf/Base/Logger.h>
#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/Image.h>

#include <SOIL.h>
#include <GL/glfw.h>

#define LOCAL_MSG "TextureGLES"

using namespace Pxf;
using namespace Pxf::Graphics;
using Util::String;

TextureGLES::TextureGLES(GraphicsDevice* _pDevice)
	: Texture(_pDevice)
	, m_TextureID(0)
	, m_LogTag(0)
{
	m_LogTag = m_pDevice->GetKernel()->CreateTag("gfx");
	m_TextureID = 0;
}

TextureGLES::~TextureGLES()
{
	Unload();
}

Math::Vec4f TextureGLES::CreateTextureSubset(float _x1, float _y1, float _x2, float _y2)
{
	float xdelta, ydelta;
	xdelta = 1.0f / (float)m_Width;
	ydelta = 1.0f / (float)m_Height;

	Math::Vec4f coords;
	/*coords.x = _x1 * xdelta;
	coords.y = 1.0f - _y1 * ydelta;
	coords.z = coords.x + _x2 * xdelta;
	coords.w = coords.y - _y2 * ydelta;*/
	coords.x = _x1 * xdelta;
	coords.y = _y1 * ydelta;
	coords.z = coords.x + _x2 * xdelta;
	coords.w = coords.y + _y2 * ydelta;
	return coords;
}
 
void TextureGLES::Load(const char* _filepath)
{
	m_Filepath = _filepath;

	Reload();

}

void TextureGLES::LoadData(const unsigned char* _datachunk, int _width, int _height, int _channels)
{
	PXFGLCHECK("TextureGLES::LoadData/Start");

	if (m_TextureID)
		Unload();

	m_Width = _width;
	m_Height = _height;
	m_Channels = _channels;
	
	/*
	GLuint tformat;
	
	if (_format == TEX_FORMAT_RGBA)
	{
		tformat = GL_RGBA;
	} else if (_format == TEX_FORMAT_A)
	{
		tformat = GL_ALPHA;
	}
	
	
	// doing it old-school
	glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, tformat, _width, _height, 0, tformat, GL_UNSIGNED_BYTE, _datachunk);
	glGenerateMipmapEXT(GL_TEXTURE_2D);
	*/
	
	if(_datachunk == NULL)
	{
		// can this be done nicer? :(
		// it's needed because you can't create a texture with soil by a NULL data pointer
		GLenum fmt = GL_RGBA;
		if (_channels == 3)
			fmt = GL_RGB;

		glGenTextures(1, &m_TextureID);
		Texture* old = m_pDevice->BindTexture(this);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, fmt, GL_UNSIGNED_BYTE, NULL);
		glGenerateMipmapEXT(GL_TEXTURE_2D);
		m_pDevice->BindTexture(old);
	}
	else
	{	
		m_TextureID = SOIL_create_OGL_texture(
			_datachunk,
			m_Width, m_Height, m_Channels,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS
			);
	}
	
	if( m_TextureID == 0)
	{
		m_pDevice->GetKernel()->Log(m_LogTag | Logger::IS_CRITICAL, "SOIL failed to load image: '%s';", SOIL_last_result() );
		PXFGLCHECK("TextureGLES::LoadData/End");
		return;
	}
	PXFGLCHECK("TextureGLES::LoadData/End");
}

void TextureGLES::UpdateData(const unsigned char* _datachunk, int _offsetx, int _offsety, int _width, int _height)
{
	PXFGLCHECK("TextureGLES::UpdateData/Start");
	if (!_datachunk)
	{
		m_pDevice->GetKernel()->Log(m_LogTag | Logger::IS_CRITICAL, "Invalid dataptr in UpdateData");
		return;
	}

	GLenum fmt = GL_RGBA;
	if (m_Channels == 3)
		fmt = GL_RGB;

	Texture* old = m_pDevice->BindTexture(this);
	glTexSubImage2D(GL_TEXTURE_2D, 0, _offsetx, _offsety, _width, _height, fmt, GL_UNSIGNED_BYTE, _datachunk);
	m_pDevice->BindTexture(old);
	PXFGLCHECK("TextureGLES::UpdateData/Start");
}

void TextureGLES::Unload()
{
	PXFGLCHECK("TextureGLES::Unload/Start");
	glDeleteTextures( 1, &m_TextureID );
	m_TextureID = 0;
	PXFGLCHECK("TextureGLES::Unload/End");
}

void TextureGLES::Reload()
{
	PXFGLCHECK("TextureGLES::Reload/Start");
	if (m_TextureID != 0)
	{
		Unload();
	}

	Resource::ResourceManager* res = GetDevice()->GetKernel()->GetResourceManager();
	Resource::Image* img = res->Acquire<Resource::Image>(m_Filepath.c_str());
	
	if (!img)
	{
		m_pDevice->GetKernel()->Log(m_LogTag | Logger::IS_CRITICAL, "Failed to load file '%s'", m_Filepath.c_str());
		PXFGLCHECK("TextureGLES::Reload/End");
		return;
	}
	
	m_TextureID = SOIL_create_OGL_texture(
		img->Ptr(),
		img->Width(), img->Height(), img->Channels(),
		SOIL_CREATE_NEW_ID,
		0);
	
	m_Width = img->Width();
	m_Height = img->Height();
	m_Channels = img->Channels();
		
	res->Release(img);
	
	if (m_TextureID == 0)
		m_pDevice->GetKernel()->Log(m_LogTag | Logger::IS_CRITICAL, "Failed to create texture for '%s'", m_Filepath.c_str());
	PXFGLCHECK("TextureGLES::Reload/End");
}

int TextureGLES::GetWidth()
{
	return m_Width;
}

int TextureGLES::GetHeight()
{
	return m_Height;
}

int TextureGLES::GetChannels()
{
	return m_Channels;
}

unsigned int TextureGLES::GetTextureID()
{
	return (unsigned int)m_TextureID;
}


void TextureGLES::SetMagFilter(TextureFilter _Filter)
{
	PXFGLCHECK("TextureGLES::SetMagFilter/Start");
	GLint param = GL_NEAREST;

	// use a lut
	if (_Filter == TEX_FILTER_NEAREST)
		param = GL_NEAREST;
	else if (_Filter == TEX_FILTER_LINEAR)
		param = GL_LINEAR;
	else
		m_pDevice->GetKernel()->Log(m_LogTag | Logger::IS_WARNING, "invalid mag filter, using 'nearest'");
	
	Texture* old = m_pDevice->BindTexture(this);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
	m_pDevice->BindTexture(old);
	PXFGLCHECK("TextureGLES::SetMagFilter/End");
}

void TextureGLES::SetMinFilter(TextureFilter _Filter)
{
	PXFGLCHECK("TextureGLES::SetMinFilter/Start");
	GLint param = GL_NEAREST;

	// use a lut
	if	  (_Filter == TEX_FILTER_NEAREST) param = GL_NEAREST;
	else if (_Filter == TEX_FILTER_LINEAR)  param = GL_LINEAR;
	else if (_Filter == TEX_FILTER_LINEAR_MIPMAP_LINEAR)  param = GL_LINEAR_MIPMAP_LINEAR;
	else if (_Filter == TEX_FILTER_LINEAR_MIPMAP_NEAREST)  param = GL_LINEAR_MIPMAP_NEAREST;
	else if (_Filter == TEX_FILTER_NEAREST_MIPMAP_LINEAR)  param = GL_NEAREST_MIPMAP_LINEAR;
	else if (_Filter == TEX_FILTER_NEAREST_MIPMAP_NEAREST)  param = GL_NEAREST_MIPMAP_NEAREST;
	else	m_pDevice->GetKernel()->Log(m_LogTag | Logger::IS_WARNING, "invalid mag filter, using GL_NEAREST");

	Texture* old = m_pDevice->BindTexture(this);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);
	m_pDevice->BindTexture(old);
	PXFGLCHECK("TextureGLES::SetMinFilter/End");
}

void TextureGLES::SetClampMethod(TextureClampMethod _Method)
{
	PXFGLCHECK("TextureGLES::SetClampMethod/Start");
	GLint m = 0;
	switch(_Method)
	{
	case TEX_CLAMP: m = GL_CLAMP; break;
	case TEX_CLAMP_TO_EDGE: m = GL_CLAMP_TO_EDGE; break;
	case TEX_REPEAT: m = GL_REPEAT; break;
	default:
		PXF_ASSERT(false, "No such clamp method");
	}
	Texture* old = m_pDevice->BindTexture(this);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m);
	m_pDevice->BindTexture(old);
	PXFGLCHECK("TextureGLES::SetClampMethod/Start");
}
