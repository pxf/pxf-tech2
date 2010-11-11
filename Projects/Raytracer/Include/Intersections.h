#ifndef _INTERSECTIONS_H_
#define _INTERSECTIONS_H_

#include <Pxf/Math/Vector.h>

struct ray_t
{
	Vec3f o;
	Vec3f d;
};

struct intersection_response_t
{
	Vec3f p;
	Vec3f n;
};

// intersection tests
bool ray_sphere(Vec3f *c, float r, ray_t *ray, intersection_response_t* resp);
bool ray_plane(Vec3f *c, Vec3f *n, ray_t *ray, intersection_response_t* resp);

#endif /* _INTERSECTIONS_H_ */
