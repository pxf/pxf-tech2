#include <Pxf/Base/Config.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Kernel.h>

#include <cstdio>

#include <cstdarg>

using namespace Pxf;

bool Pxf::Assert(bool _Expression, int _Line, const char* _File, const char* _Message, ...)
{
	if (!_Expression)
	{
		char Buffer[4092];
		
		va_list va;
		va_start(va, _Message);
		vsprintf(Buffer, _Message, va);
		va_end(va);
		
		printf("Assertion at %s:%d: %s\n", _File, _Line, Buffer);
		return false;
	}
	return true;
}

void Pxf::Message(const char* _SubSystem, const char* _Message, ...)
{
	char Buffer[4092];

	va_list va;
	va_start(va, _Message);
	vsprintf(Buffer, _Message, va);
	va_end(va);

	Kernel::GetInstance()->Log(0, Buffer);
}