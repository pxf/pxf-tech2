#ifndef _PXF_UTIL_ARRAY_H_
#define _PXF_UTIL_ARRAY_H_

#include <vector>

namespace Pxf {
namespace Util {

	template<typename T>
	class Array : public std::vector<T>
	{

	};

} // Util
} // Pxf

#endif //_PXF_UTIL_ARRAY_H_


