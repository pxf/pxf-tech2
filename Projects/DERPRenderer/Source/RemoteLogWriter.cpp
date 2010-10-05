#include <RemoteLogWriter.h>
#include <Pxf/Network/Server.h>

bool Derp::RemoteLogWriter::WriteImpl(unsigned int _Tag, const char** _TagTable
					, unsigned int _TagTableSize, const char* _SrcBuffer
					, unsigned int _SrcLength)
{
	unsigned short TagID = _Tag & 0x0000FFFF;
	const char* TagName = 0;

	if (!IsRegistered(_Tag) || TagID >= _TagTableSize-1)
		TagName = _TagTable[0];
	else
		TagName = _TagTable[TagID];


	// Should format and add debug/warning/critical-flags
	m_Server->SendAllL(m_NetLogTag, _SrcBuffer, _SrcLength);

	return true;
}