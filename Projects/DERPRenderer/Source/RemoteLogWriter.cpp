#include <RemoteLogWriter.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/String.h>
#include <Pxf/Network/NetworkDevice.h>
#include <Pxf/Network/Server.h>
#include <Pxf/Network/Packet.h>

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
	//char buff[4096];
	const char* type = 0;
	if (_Tag & Logger::IS_DEBUG)
		type = "debug";
	else if (_Tag & Logger::IS_INFORMATION)
		type = "information";
	else if (_Tag & Logger::IS_WARNING)
		type = "warning";
	else if (_Tag & Logger::IS_CRITICAL)
		type = "critical";
	else
		type = "unknown";

	//Pxf::Format(buff, "(%s) %s | %s", TagName, type, _SrcBuffer);
	//m_Server->SendAllID("rlog", m_NetLogTag, buff, Pxf::StringLength(buff));

	Pxf::Network::Packet* logpacket = m_Device->CreateEmptyPacket("rlog", m_NetLogTag);
	logpacket->PushString(TagName, Pxf::StringLength(TagName) + 1);
	logpacket->PushString(type, Pxf::StringLength(type) + 1);
	logpacket->PushString(_SrcBuffer, Pxf::StringLength(_SrcBuffer) + 1);
	m_Server->SendAllPacket(logpacket);

	return true;
}