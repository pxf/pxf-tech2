#include <Pxf/Kernel.h>
#include <Pxf/Util/String.h>
#include <Pxf/Modules/pri/VertexBufferGL2.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Stream.h>
#include <Pxf/Base/Logger.h>
#include <Pxf/Base/Memory.h>

#include <Pxf/Modules/pri/OpenGL.h>
#include <Pxf/Modules/pri/OpenGLUtils.h>
#include <Pxf/Modules/pri/UniGL.h>

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
	, m_LogTag(0)
{
	m_NumCustomVertexAttributes = 0;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &m_NumCustomVertexAttributes);
	m_CustomVertexAttributes = new AttributeData[m_NumCustomVertexAttributes]();

	m_LogTag = m_pDevice->GetKernel()->CreateTag("gfx");
	if (_VertexBufferLocation == VB_LOCATION_GPU)
	{
		if (!GLEW_VERSION_1_5 && !glewIsSupported("GL_VERSION_1_4  ARB_vertex_buffer_object")
			&& !glBindBufferARB && !glBindBuffer)
		{
			m_pDevice->GetKernel()->Log(m_LogTag | Logger::IS_WARNING, "Vertex Buffer can not use GPU memory, using system memory instead.");
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
			delete [] (char*)m_InterleavedData;
	}
}


void VertexBufferGL2::_PreDraw()
{
	if(m_VertexAttributes.NumComponents == 0)
		return;

	PXFGLCHECK("VertexBufferGL2::_PreDraw/Start");
	unsigned int BufferOffset = 0;
	if (m_VertexBufferLocation == VB_LOCATION_GPU)
	{
		GL::BindBuffer(GL::ARRAY_BUFFER, (GLuint) m_BufferObjectId);
	}
	else
	{
		BufferOffset = (size_t)m_InterleavedData;
	}

	PXF_ASSERT(m_Attributes & VB_VERTEX_DATA, "Attempt to draw without vertex data.");
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(m_VertexAttributes.NumComponents, GL_FLOAT, m_VertexSize, GL::BufferObjectPtr(BufferOffset + m_VertexAttributes.StrideOffset));


	if(m_Attributes & VB_NORMAL_DATA)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, m_VertexSize, GL::BufferObjectPtr(BufferOffset + m_NormalAttributes.StrideOffset));
	}

	if(m_Attributes & VB_TEXCOORD0_DATA)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(m_TexCoordAttributes[0].NumComponents, GL_FLOAT, m_VertexSize, GL::BufferObjectPtr(BufferOffset + m_TexCoordAttributes[0].StrideOffset));
	}

	if(m_Attributes & VB_TEXCOORD1_DATA)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glClientActiveTexture(GL_TEXTURE1);
		glTexCoordPointer(m_TexCoordAttributes[1].NumComponents, GL_FLOAT, m_VertexSize, GL::BufferObjectPtr(BufferOffset + m_TexCoordAttributes[1].StrideOffset)); 
	}

	if(m_Attributes & VB_TEXCOORD2_DATA)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glClientActiveTexture(GL_TEXTURE2);
		glTexCoordPointer(m_TexCoordAttributes[2].NumComponents, GL_FLOAT, m_VertexSize, GL::BufferObjectPtr(BufferOffset + m_TexCoordAttributes[2].StrideOffset)); 
	}

	if(m_Attributes & VB_TEXCOORD3_DATA)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glClientActiveTexture(GL_TEXTURE3);
		glTexCoordPointer(m_TexCoordAttributes[3].NumComponents, GL_FLOAT, m_VertexSize, GL::BufferObjectPtr(BufferOffset + m_TexCoordAttributes[3].StrideOffset)); 
	}

	if(m_Attributes & VB_TEXCOORD4_DATA)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glClientActiveTexture(GL_TEXTURE4);
		glTexCoordPointer(m_TexCoordAttributes[4].NumComponents, GL_FLOAT, m_VertexSize, GL::BufferObjectPtr(BufferOffset + m_TexCoordAttributes[4].StrideOffset)); 
	}

	if(m_Attributes & VB_TEXCOORD5_DATA)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glClientActiveTexture(GL_TEXTURE5);
		glTexCoordPointer(m_TexCoordAttributes[5].NumComponents, GL_FLOAT, m_VertexSize, GL::BufferObjectPtr(BufferOffset + m_TexCoordAttributes[5].StrideOffset)); 
	}

	if(m_Attributes & VB_TEXCOORD6_DATA)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glClientActiveTexture(GL_TEXTURE6);
		glTexCoordPointer(m_TexCoordAttributes[6].NumComponents, GL_FLOAT, m_VertexSize, GL::BufferObjectPtr(BufferOffset + m_TexCoordAttributes[6].StrideOffset)); 
	}

	if(m_Attributes & VB_TEXCOORD7_DATA)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glClientActiveTexture(GL_TEXTURE7);
		glTexCoordPointer(m_TexCoordAttributes[7].NumComponents, GL_FLOAT, m_VertexSize, GL::BufferObjectPtr(BufferOffset + m_TexCoordAttributes[7].StrideOffset)); 
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

	for (int i=0; i < m_NumCustomVertexAttributes; i++)
	{
		AttributeData* data = &m_CustomVertexAttributes[i];
		if (data->AttributeIndex >= 0)
		{
			GLenum gltype = LookupDDT(data->AttributeType);
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, data->NumComponents, gltype, data->AttributeNormalized,
								  m_VertexSize, GL::BufferObjectPtr(BufferOffset + data->StrideOffset));
		}
	}

	PXFGLCHECK("VertexBufferGL2::_PreDraw/End");
}

void VertexBufferGL2::_PostDraw()
{
	PXFGLCHECK("VertexBufferGL2::_PostDraw/Start");
	if (m_VertexBufferLocation == VB_LOCATION_GPU)
	{
		GL::BindBuffer(GL::ARRAY_BUFFER, 0);
	}

	glDisableClientState(GL_VERTEX_ARRAY);


	if(m_Attributes & VB_NORMAL_DATA)
	{
		glDisableClientState(GL_NORMAL_ARRAY);
	}

	if(m_Attributes & VB_TEXCOORD0_DATA)
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

	for (int i=0; i < m_NumCustomVertexAttributes; i++)
	{
		AttributeData* data = &m_CustomVertexAttributes[i];
		if (data->AttributeIndex >= 0)
		{
			glDisableVertexAttribArray(i);
		}
	}

	PXFGLCHECK("VertexBufferGL2::_PostDraw/End");
}



void VertexBufferGL2::CreateNewBuffer(uint32 _NumVertices, uint32 _VertexSize)
{
	PXFGLCHECK("VertexBufferGL2::CreateNewBuffer/Start");
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
	PXFGLCHECK("VertexBufferGL2::CreateNewBuffer/End");
}

void VertexBufferGL2::CreateFromBuffer(void* _Buffer,uint32 _NumVertices, uint32 _VertexSize)
{
	PXFGLCHECK("VertexBufferGL2::CreateFromBuffer/Start");
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
	PXFGLCHECK("VertexBufferGL2::CreateFromBuffer/End");
}

void VertexBufferGL2::UpdateData(void* _Buffer, uint32 _Count, uint32 _Offset)
{
	PXF_ASSERT(m_IsMapped == false, "Vertex buffer is busy.");
	PXFGLCHECK("VertexBufferGL2::UpdateData/Start");
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
	PXFGLCHECK("VertexBufferGL2::UpdateData/End");
}

void* VertexBufferGL2::MapData(VertexBufferAccessFlag _AccessFlag)
{
	PXFGLCHECK("VertexBufferGL2::MapData/Start");
	if (m_VertexBufferLocation == VB_LOCATION_GPU && m_BufferObjectId != 0)
	{
		GLuint access = LookupAccessFlag(_AccessFlag);
		GL::BindBuffer(GL::ARRAY_BUFFER, (GLuint) m_BufferObjectId);
		void* data = GL::MapBuffer(GL::ARRAY_BUFFER, access);
		GL::BindBuffer(GL::ARRAY_BUFFER, 0);
		m_IsMapped = true;
		PXFGLCHECK("VertexBufferGL2::MapData/End");
		return data;
	}
	else if (m_VertexBufferLocation == VB_LOCATION_SYS && m_InterleavedData != 0)
	{
		m_IsMapped = true;
		PXFGLCHECK("VertexBufferGL2::MapData/End");
		return m_InterleavedData;
	}

	PXFGLCHECK("VertexBufferGL2::MapData/End");
	return NULL;
}

void VertexBufferGL2::UnmapData()
{
	PXFGLCHECK("VertexBufferGL2::UnmapData/Start");
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
	PXFGLCHECK("VertexBufferGL2::UnmapData/End");
}

void VertexBufferGL2::SetCustomData(uint32 _Index, DeviceDataType _Type, bool _Normalized, uint8 _StrideOffset, uint8 _NumComponents)
{
	PXF_ASSERT(_Index >= 0 && _Index < m_NumCustomVertexAttributes, "Invalid index");
	m_CustomVertexAttributes[_Index].NumComponents = _NumComponents;
	m_CustomVertexAttributes[_Index].StrideOffset = _StrideOffset;
	m_CustomVertexAttributes[_Index].AttributeIndex = _Index;
	m_CustomVertexAttributes[_Index].AttributeType = _Type;
	m_CustomVertexAttributes[_Index].AttributeNormalized = _Normalized;
}