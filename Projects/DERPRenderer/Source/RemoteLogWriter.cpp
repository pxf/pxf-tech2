#include <RemoteLogWriter.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/String.h>
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
	char buff[4096];
	const char* type = 0;
	if (_Tag & Logger::IS_DEBUG)
		type = "debug";
	else if (_Tag & Logger::IS_INFORMATION)
		type = "info";
	else if (_Tag & Logger::IS_WARNING)
		type = "warn";
	else if (_Tag & Logger::IS_CRITICAL)
		type = "fatal";

	Pxf::Format(buff, "%s | %s", type, _SrcBuffer);

	m_Server->SendAllL(m_NetLogTag, _SrcBuffer, Pxf::StringLength(buff));

	return true;
}