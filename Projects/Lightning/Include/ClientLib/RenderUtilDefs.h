#ifndef _RENDERUTILDEFS_H_
#define _RENDERUTILDEFS_H_

#include <Pxf/Math/Vector.h>

struct vertex_t
{
	Pxf::Math::Vec3f v;		// 12
	Pxf::Math::Vec3f n;		// 12
	Pxf::Math::Vec2f uv;	// 8
};	// 32

#endif /* _RENDERUTILS_H_ */