#include <Pxf/Pxf.h>
#include <Pxf/Util/String.h>
#include <Pxf/Modules/pri/VertexBufferGL2.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Stream.h>

#include <Pxf/Modules/pri/OpenGL.h>
#include <Pxf/Modules/pri/TypeTraits.h>

#define LOCAL_MSG "VertexBuffer"

using namespace Pxf;
using namespace Pxf::Graphics;
using Util::String;


static GLuint LookupUsageFlag(VertexBufferUsageFlag _BufferUsageFlag)
{
	switch(_BufferUsageFlag)
	{
	case VB_USAGE_STATIC_DRAW: return GL_STATIC_DRAW;
	case VB_USAGE_STATIC_READ: return GL_STATIC_READ;
	case VB_USAGE_STATIC_COPY: return GL_STATIC_COPY;
	case VB_USAGE_DYNAMIC_DRAW: return GL_DYNAMIC_DRAW;
	case VB_USAGE_DYNAMIC_READ: return GL_DYNAMIC_READ;
	case VB_USAGE_DYNAMIC_COPY: return GL_DYNAMIC_COPY;
	case VB_USAGE_STREAM_DRAW: return GL_STREAM_DRAW;
	case VB_USAGE_STREAM_READ: return GL_STREAM_READ;
	case VB_USAGE_STREAM_COPY: return GL_STREAM_COPY;
	}
	return 0;
}

static GLuint LookupAccessFlag(VertexBufferAccessFlag _BufferAccessFlag)
{
	switch(_BufferAccessFlag)
	{
	case VB_ACCESS_READ_ONLY: return GL_READ_ONLY;
	case VB_ACCESS_WRITE_ONLY: return GL_WRITE_ONLY;
	case VB_ACCESS_READ_WRITE: return GL_READ_WRITE;
	}

	return 0;
}


VertexBufferGL2::VertexBufferGL2(GraphicsDevice* _pDevice, VertexBufferLocation _VertexBufferLocation, VertexBufferUsageFlag _VertexBufferUsageFlag)
	: VertexBuffer(_pDevice, _VertexBufferLocation, _VertexBufferUsageFlag)
	, m_BufferObjectId(0)
{
	if (_VertexBufferLocation == VB_LOCATION_GPU)
	{
		/* OpenGL 1.5 */
		if (GLEW_VERSION_1_5)
		{
			Message("VertexBuffer", "Can use OpenGL 1.5!");
		}
		/* OpenGL 1.4 with VBO support as extension */
		else if (glewIsSupported("GL_VERSION_1_4  ARB_vertex_buffer_object"))
		{
			Message("VertexBuffer", "Can use OpenGL 1.4 with VBO support as extension");
		}
		else if (glBindBufferARB != 0)
		{
			Message("VertexBuffer", "VBO support might work");
		}
		else
		{
			Message("VertexBuffer", "Can not use GPU memory (?)");
			m_VertexBufferLocation = VB_LOCATION_SYS;
		}
	}
}

VertexBufferGL2::~VertexBufferGL2()
{
	if (m_VertexBufferLocation == VB_LOCATION_GPU)
	{
		if (m_BufferObjectId)
			GL::DeleteBuffers(1, (GLuint*) &m_BufferObjectId);
	}
	else
	{
		if (m_InterleavedData)
			delete [] m_InterleavedData;
	}
}


void VertexBufferGL2::_PreDraw()
{
	unsigned int BufferOffset = 0;
	if (m_VertexBufferLocation == VB_LOCATION_GPU)
	{
		GL::BindBuffer(GL::ARRAY_BUFFER, (GLuint) m_BufferObjectId);
	}
	else
	{
		BufferOffset = (unsigned int)m_InterleavedData;
	}

	PXF_ASSERT(m_Attributes & VB_VERTEX_DATA, "Attempt to draw without vertex data.");
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(m_VertexAttributes.NumComponents, GL_FLOAT, m_VertexSize, GL::BufferObjectPtr(BufferOffset + m_VertexAttributes.StrideOffset));


	if(m_Attributes & VB_NORMAL_DATA)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, m_VertexSize, GL::BufferObjectPtr(BufferOffset + m_NormalAttributes.StrideOffset));
	}

	if(m_Attributes & VB_TEXCOORD_DATA)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(m_TexCoordAttributes.NumComponents, GL_FLOAT, m_VertexSize, GL::BufferObjectPtr(BufferOffset + m_TexCoordAttributes.StrideOffset));
	}

	if(m_Attributes & VB_COLOR_DATA)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(m_ColorAttributes.NumComponents, GL_FLOAT, m_VertexSize, GL::BufferObjectPtr(BufferOffset + m_ColorAttributes.StrideOffset));
	}

	if(m_Attributes & VB_INDEX_DATA)
	{
		glEnableClientState(GL_INDEX_ARRAY);
		glIndexPointer(GL_FLOAT, m_VertexSize, GL::BufferObjectPtr(BufferOffset + m_ColorAttributes.StrideOffset));
	}

	if(m_Attributes & VB_EDGEFLAG_DATA)
	{
		glEnableClientState(GL_EDGE_FLAG_ARRAY);
		glEdgeFlagPointer(m_VertexSize, GL::BufferObjectPtr(BufferOffset + m_EdgeFlagAttributes.StrideOffset));
	}
}

void VertexBufferGL2::_PostDraw()
{
	if (m_VertexBufferLocation == VB_LOCATION_GPU)
	{
		GL::BindBuffer(GL::ARRAY_BUFFER, 0);
	}

	glDisableClientState(GL_VERTEX_ARRAY);


	if(m_Attributes & VB_NORMAL_DATA)
	{
		glDisableClientState(GL_NORMAL_ARRAY);
	}

	if(m_Attributes & VB_TEXCOORD_DATA)
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	if(m_Attributes & VB_COLOR_DATA)
	{
		glDisableClientState(GL_COLOR_ARRAY);
	}

	if(m_Attributes & VB_INDEX_DATA)
	{
		glDisableClientState(GL_INDEX_ARRAY);
	}

	if(m_Attributes & VB_EDGEFLAG_DATA)
	{
		glDisableClientState(GL_EDGE_FLAG_ARRAY);
	}
}



void VertexBufferGL2::CreateNewBuffer(uint32 _NumVertices, uint32 _VertexSize)
{
	if (m_InterleavedData != 0 || m_BufferObjectId != 0)
		return;

	if (m_VertexBufferLocation == VB_LOCATION_GPU)
	{
		GLuint usage = LookupUsageFlag(m_VertexBufferUsageFlag);
		GL::GenBuffers(1, (GLuint*)&m_BufferObjectId);
		GL::BindBuffer(GL::ARRAY_BUFFER, (GLuint) m_BufferObjectId);
		GL::BufferData(GL::ARRAY_BUFFER, _NumVertices * _VertexSize, 0, usage);
		GL::BindBuffer(GL::ARRAY_BUFFER, 0);
	}
	else
	{
		m_InterleavedData = new char[_NumVertices * _VertexSize];
	}

	m_VertexCount = _NumVertices;
	m_VertexSize = _VertexSize;
	m_ByteCount = _NumVertices * _VertexSize;
}

void VertexBufferGL2::CreateFromBuffer(void* _Buffer,uint32 _NumVertices, uint32 _VertexSize)
{
	if (m_InterleavedData != 0 && m_BufferObjectId != 0)
		return;

	if (m_VertexBufferLocation == VB_LOCATION_GPU)
	{
		// Copy to gpu memory
		GLuint usage = LookupUsageFlag(m_VertexBufferUsageFlag);
		GL::GenBuffers(1, (GLuint*)&m_BufferObjectId);
		GL::BindBuffer(GL::ARRAY_BUFFER, (GLuint) m_BufferObjectId);
		GL::BufferData(GL::ARRAY_BUFFER, _NumVertices * _VertexSize, _Buffer, usage);
		GL::BindBuffer(GL::ARRAY_BUFFER, 0);
	}
	else
	{
		m_InterleavedData = new char[_NumVertices * _VertexSize];
		MemoryStream stream((char*)m_InterleavedData, _NumVertices * _VertexSize);
		stream.Write(_Buffer, _NumVertices * _VertexSize);
	}

	m_VertexCount = _NumVertices;
	m_VertexSize = _VertexSize;
	m_ByteCount = _NumVertices * _VertexSize;
}

void VertexBufferGL2::UpdateData(void* _Buffer, uint32 _Count, uint32 _Offset)
{
	if (m_VertexBufferLocation == VB_LOCATION_GPU && m_BufferObjectId != 0)
	{
		GL::BindBuffer(GL::ARRAY_BUFFER, (GLuint) m_BufferObjectId);
		GL::BufferSubData(GL::ARRAY_BUFFER, _Offset, _Count, _Buffer);
		GL::BindBuffer(GL::ARRAY_BUFFER, 0);
	}
	else if (m_VertexBufferLocation == VB_LOCATION_SYS && m_InterleavedData != 0)
	{
		MemoryStream stream((char*)m_InterleavedData, m_ByteCount);
		stream.SeekTo(_Offset);
		stream.Write(_Buffer, _Count);
	}
}

void* VertexBufferGL2::MapData(VertexBufferAccessFlag _AccessFlag)
{
	if (m_VertexBufferLocation == VB_LOCATION_GPU && m_BufferObjectId != 0)
	{
		GLuint access = LookupAccessFlag(_AccessFlag);
		GL::BindBuffer(GL::ARRAY_BUFFER, (GLuint) m_BufferObjectId);
		void* data = GL::MapBuffer(GL::ARRAY_BUFFER, access);
		GL::BindBuffer(GL::ARRAY_BUFFER, 0);
		m_IsMapped = true;
		return data;
	}
	else if (m_VertexBufferLocation == VB_LOCATION_SYS && m_InterleavedData != 0)
	{
		m_IsMapped = true;
		return m_InterleavedData;
	}

	return NULL;
}

void VertexBufferGL2::UnmapData()
{

	if (m_VertexBufferLocation == VB_LOCATION_GPU && m_BufferObjectId != 0)
	{
		GL::BindBuffer(GL::ARRAY_BUFFER, (GLuint) m_BufferObjectId);
		GL::UnmapBuffer(GL::ARRAY_BUFFER);
		GL::BindBuffer(GL::ARRAY_BUFFER, 0);
		m_IsMapped = false;
	}
	
	if (m_VertexBufferLocation == VB_LOCATION_SYS && m_InterleavedData != 0)
	{
		m_IsMapped = false;
	}
}

