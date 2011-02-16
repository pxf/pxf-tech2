#ifndef _PXF_UTIL_MAP_H_
#define _PXF_UTIL_MAP_H_

#include <map>

namespace Pxf {
namespace Util {

// Implement Hash-map later. R/B-tree is good enough for now. :]

template<typename K, typename V>
class Map : public std::map<K,V>
{

};

} // Util
} // Pxf

#endif //_PXF_UTIL_MAP_H_


