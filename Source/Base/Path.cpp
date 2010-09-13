#include <Pxf/Base/Path.h>
#include <Pxf/Base/String.h>

using namespace Pxf;

const char* Pxf::PathExt(const char* _FilePath)
{
	return StringFindRev(_FilePath, '.') + 1;
}