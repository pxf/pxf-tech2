#ifndef _PXF_UTIL_LIST_H_
#define _PXF_UTIL_LIST_H_

#include <list>

namespace Pxf {
namespace Util {

template<typename T>
class List : public std::list<T>
{

};

} // Util
} // Pxf

#endif //_PXF_UTIL_LIST_H_


