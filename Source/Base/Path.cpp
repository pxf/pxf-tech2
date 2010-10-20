#include <Pxf/Base/Path.h>
#include <Pxf/Base/String.h>

using namespace Pxf;

const char* Pxf::PathExt(const char* _FilePath)
{
	return StringFindRev(_FilePath, '.') + 1;
}

const char* Pxf::PathFileName(const char* _FilePath)
{
	const char* pos = StringFindRev2(_FilePath, '/', '\\');
	return pos == 0 ? _FilePath : pos + 1;
}