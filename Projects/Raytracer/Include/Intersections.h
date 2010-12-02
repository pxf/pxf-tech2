#ifndef _INTERSECTIONS_H_
#define _INTERSECTIONS_H_

#include <Pxf/Base/Debug.h>
#include <Pxf/Math/Vector.h>
#include <algorithm>

using namespace std;

class Prim;

struct ray_t
{
	Pxf::Math::Vec3f o;
	Pxf::Math::Vec3f d;
};

struct intersection_response_t
{
	Pxf::Math::Vec3f p;
	Pxf::Math::Vec3f n;
	float depth;
	intersection_response_t() {p = Pxf::Math::Vec3f(0.0f); n = Pxf::Math::Vec3f(0.0f); depth = 0.0f;}
};

struct aabb {
	aabb operator+(const aabb& box)
	{
		aabb new_box;
		new_box.pos = Pxf::Math::Vec3f(min(pos.x,box.pos.x),min(pos.y,box.pos.y),min(pos.z,box.pos.z));
		new_box.size = Pxf::Math::Vec3f(max(pos.x + size.x,box.pos.x + box.size.x),
									max(pos.y + size.y,box.pos.y + box.size.y),
									max(pos.z + size.z,box.pos.z + box.size.z));
		
		new_box.size = new_box.size - new_box.pos;
		return new_box;
	}

	Pxf::Math::Vec3f pos;
	Pxf::Math::Vec3f size;
};

// intersection tests
bool ray_aabb(ray_t* ray,aabb* box,intersection_response_t* resp);
bool ray_triangle(Pxf::Math::Vec3f* data,ray_t* ray, intersection_response_t* resp);
bool ray_sphere(Pxf::Math::Vec3f *c, float r, ray_t *ray, intersection_response_t* resp);
bool ray_plane(Pxf::Math::Vec3f *c, Pxf::Math::Vec3f *n, ray_t *ray, intersection_response_t* resp);

// helpers
aabb CalcAABB(Prim** _Primitives, int _NbrPrim);	// calculate an aabb from a set of primitives
aabb CalcAABB(const Prim& _Primitive);			// calc aabb from one primitive

#endif /* _INTERSECTIONS_H_ */
