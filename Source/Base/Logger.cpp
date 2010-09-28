#include <Pxf/Base/Logger.h>
#include <Pxf/Base/Memory.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Utils.h>
#include <cstdio>

#ifdef CONF_PLATFORM_MACOSX
#include <cstdarg>
#endif

using namespace Pxf;

bool Logger::Write(unsigned int _Tag, const char** _TagTable, unsigned int _TagTableSize, const char* _Message)
{
	return WriteImpl(_Tag, _TagTable, _TagTableSize, _Message, StringLength(_Message));
}

bool Logger::IsDebug(unsigned int _Tag)
{
	return _Tag & IS_DEBUG;
}

bool Logger::IsInformation(unsigned int _Tag)
{
	return _Tag & IS_INFORMATION;
}

bool Logger::IsWarning(unsigned int _Tag)
{
	return _Tag & IS_WARNING;
}

bool Logger::IsCritical(unsigned int _Tag)
{
	return _Tag & IS_CRITICAL;
}

bool Logger::IsRegistered(unsigned int _Tag)
{
	return _Tag & IS_REGISTERED;
}


// Logger for stdout

bool StdLogger::WriteImpl(unsigned int _Tag, const char** _TagTable, unsigned int _TagTableSize, const char* _SrcBuffer, unsigned int _SrcLength)
{
	unsigned short TagID = _Tag & 0x0000FFFF;
	const char* TagName = 0;

	if (!IsRegistered(_Tag) || TagID >= _TagTableSize-1)
		TagName = _TagTable[0];
	else
		TagName = _TagTable[TagID];
	
	return printf("[%s] %s\n", TagName, _SrcBuffer) > 0;
}