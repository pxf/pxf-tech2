#include <Pxf/Base/Stream.h>

#include <cstdio>
#include <cstring>

using namespace Pxf;

bool FileStream::Open(const char* _FileName, const char* _Mode)
{
	m_Handle = (void*) fopen(_FileName, _Mode);
	if (IsValid())
	{
		m_Offset = 0;
		
		uint InitialPosition = ftell((FILE*)m_Handle);
		fseek((FILE*)m_Handle, 0, SEEK_END);
		m_Size = m_Remaining = ftell((FILE*)m_Handle);
		fseek((FILE*)m_Handle, InitialPosition, SEEK_SET);
	}
	return IsValid();
}

bool FileStream::Close()
{
	PXF_ASSERT(IsValid(), "FileStream::Close: Invalid filehandle");
	bool Ret = fclose((FILE*)m_Handle) >= 0;
	m_Handle = NULL;
	return Ret;
}

uint FileStream::Read(void* _Buffer, uint _Count)
{
	PXF_ASSERT(IsValid(), "FileStream::Read: Invalid handle");
	PXF_ASSERT(m_Offset + _Count <= m_Size, "FileStream::Read: out of range");
	uint BytesRead = fread(_Buffer, 1, _Count, (FILE*)m_Handle);

	if (ferror((FILE*)m_Handle))
		return 0;

	m_Offset += BytesRead;
	m_Remaining -= BytesRead;
	return BytesRead;
}

uint FileStream::Write(const void* _Buffer, uint _Count)
{
	PXF_ASSERT(IsValid(), "FileStream::Read: Invalid handle");
	PXF_ASSERT(m_Offset + _Count <= m_Size, "FileStream:Write: out of range");
	uint BytesWritten = fwrite(_Buffer, _Count, 1, (FILE*)m_Handle);

	if (ferror((FILE*)m_Handle))
		return 0;

	m_Offset += BytesWritten;
	m_Remaining -= BytesWritten;
	return BytesWritten;
}

bool FileStream::Flush()
{
	return fflush((FILE*)m_Handle) >= 0;
}

bool FileStream::Skip(uint _Count)
{
	PXF_ASSERT(IsValid(), "FileStream::Skip: Invalid handle");
	PXF_ASSERT(m_Offset + _Count <= m_Size, "FileStream::Skip: Skipping past EOF");
	m_Offset += _Count;
	m_Remaining -= _Count;
	return fseek((FILE*)m_Handle, _Count, SEEK_CUR) >= 0;
	
}

bool FileStream::SeekTo(uint _Offset)
{
	PXF_ASSERT(IsValid(), "FileStream::SeekTo: Invalid handle");
	PXF_ASSERT(m_Offset + _Offset <= m_Size, "FileStream::SeekTo: Seeking past EOF");
	PXF_ASSERT(_Offset >= 0 && _Offset <= m_Size, "FileStream::SeekTo: Invalid offset");
	bool ret = fseek((FILE*)m_Handle, _Offset, SEEK_SET) >= 0;
	if (ret)
	{
		m_Offset = _Offset;
		m_Remaining = m_Size - _Offset;
	}
	return ret;
}

bool FileStream::SeekToStart()
{
	PXF_ASSERT(IsValid(), "FileStream::SeekToStart: Invalid handle");
	bool ret = fseek((FILE*)m_Handle, 0, SEEK_SET) >= 0;
	if (ret)
	{
		m_Offset = 0;
		m_Remaining = m_Size;
	}
	return ret;
}

bool FileStream::SeekToEnd()
{
	PXF_ASSERT(IsValid(), "FileStream::SeekToEnd: Invalid handle");
	bool ret = fseek((FILE*)m_Handle, 0, SEEK_END) >= 0;
	if (ret)
	{
		m_Offset = m_Size;
		m_Remaining = 0;
	}
	return ret;
}



//////////////////////////////////////////////////////////////////////////


uint MemoryStream::Read(void* _Buffer, uint _Count)
{
	PXF_ASSERT(IsValid(), "Invalid pointer");
	PXF_ASSERT(m_Offset + _Count <= m_Size, "Reading out of range");
	memcpy((char*)_Buffer, m_Buffer, _Count);
	m_Buffer += _Count;
	m_Offset += _Count;
	m_Remaining -= _Count;
	return true;
}


uint MemoryStream::Write(const void* _Buffer, uint _Count)
{
	PXF_ASSERT(IsValid(), "Invalid pointer");
	PXF_ASSERT(m_Offset + _Count <= m_Size, "Writing out of range");
	memcpy((char*)m_Buffer, _Buffer, _Count);
	m_Buffer += _Count;
	m_Offset += _Count;
	m_Remaining -= _Count;
	return true;
}

void MemoryStream::Skip(uint _Count)
{
	PXF_ASSERT(IsValid(), "Invalid pointer");
	PXF_ASSERT(m_Offset + _Count <= m_Size && m_Offset + _Count >= 0, "Skipping out of range");
	m_Buffer += _Count;
	m_Offset += _Count;
	m_Remaining -= _Count;
}

void MemoryStream::SeekTo(uint _Offset)
{
	PXF_ASSERT(IsValid(), "Invalid pointer");

	uint diff = _Offset - m_Offset;
	PXF_ASSERT(m_Offset + diff <= m_Size && m_Offset + diff >= 0, "Offset out of range");

	m_Buffer += diff;
	m_Remaining -= diff;
	m_Offset += diff;
}

void MemoryStream::SeekToStart()
{
	PXF_ASSERT(IsValid(), "Invalid pointer");
	m_Buffer -= m_Offset;
	m_Remaining = m_Size;
	m_Offset = 0;
}

void MemoryStream::SeekToEnd()
{
	PXF_ASSERT(IsValid(), "Invalid pointer");
	m_Buffer += m_Remaining;
	m_Remaining = 0;
	m_Offset = m_Size;
}
