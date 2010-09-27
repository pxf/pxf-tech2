#include <Pxf/Base/Logger.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Utils.h>
#include <cstdio>

#ifdef CONF_PLATFORM_MACOSX
#include <cstdarg>
#endif

using namespace Pxf;

const char* Logger::s_Tags[MAX_TAGS] = {"Undef", 0};
unsigned short Logger::s_TagCount = 1;

bool Logger::Write(unsigned int _Tag, const char* _Message)
{
	return WriteImpl(_Tag, _Message, StringLength(_Message));
}

unsigned int Logger::CreateTag(const char* _TagName, unsigned int _Options)
{
	unsigned ret = s_TagCount++;
	s_Tags[ret] = _TagName;
	ret |= _Options;
	ret |= IS_REGISTERED;
	return ret;
}

unsigned int Logger::FindTagID(const char* _TagName)
{
	for(int i = 0; i < MAX_TAGS; i++)
	{
		if (StringCompareI(_TagName, s_Tags[i]))
			return i;
	}
	return 0;
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

bool StdLogger::WriteImpl(unsigned int _Tag, const char* _SrcBuffer, unsigned int _SrcLength)
{
	unsigned short TagID = _Tag & 0x0000FFFF;
	const char* TagName = 0;

	if (!IsRegistered(_Tag) || TagID >= MAX_TAGS-1)
		TagName = s_Tags[0];
	else
		TagName = s_Tags[TagID];
	
	return printf("[%s] %s\n", TagName, _SrcBuffer) > 0;
}