#ifndef _INTERSECTIONS_H_
#define _INTERSECTIONS_H_

#include <Pxf/Base/Debug.h>
#include <Pxf/Math/Vector.h>

struct ray_t
{
	Pxf::Math::Vec3f o;
	Pxf::Math::Vec3f d;
};

struct intersection_response_t
{
	Pxf::Math::Vec3f p;
	Pxf::Math::Vec3f n;
};

// intersection tests
bool ray_sphere(Pxf::Math::Vec3f *c, float r, ray_t *ray, intersection_response_t* resp);
bool ray_plane(Pxf::Math::Vec3f *c, Pxf::Math::Vec3f *n, ray_t *ray, intersection_response_t* resp);

#endif /* _INTERSECTIONS_H_ */
