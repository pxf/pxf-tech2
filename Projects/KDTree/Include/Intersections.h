#ifndef _INTERSECTIONS_H_
#define _INTERSECTIONS_H_

#include <Pxf/Base/Debug.h>
#include <Pxf/Math/Vector.h>
#include <Common.h>


struct ray_t
{
	Pxf::Math::Vec3f o;
	Pxf::Math::Vec3f d;
	Pxf::Math::Vec3f inv_d;
}; 

struct intersection_response_t
{
	Pxf::Math::Vec3f p;
	Pxf::Math::Vec3f n;
	float depth;
};

struct aabb_intersection_response_t : public intersection_response_t
{
	float t_min;
	float t_max;
};

struct aabb {
	aabb operator+(const aabb& box)
	{
		aabb new_box;
		new_box.pos = Pxf::Math::Vec3f(Min(pos.x,box.pos.x),Min(pos.y,box.pos.y),Min(pos.z,box.pos.z));
		new_box.size = Pxf::Math::Vec3f(Max(pos.x + size.x,box.pos.x + box.size.x),
									Max(pos.y + size.y,box.pos.y + box.size.y),
									Max(pos.z + size.z,box.pos.z + box.size.z));
		
		new_box.size = new_box.size - new_box.pos;
		return new_box;
	}

	Pxf::Math::Vec3f pos;
	Pxf::Math::Vec3f size;
};

// intersection tests
//bool ray_aabb(ray_t* ray,aabb* box);
bool ray_aabb(ray_t* ray,aabb* box,intersection_response_t* resp);
bool ray_triangle(Pxf::Math::Vec3f* data,ray_t* ray, intersection_response_t* resp);
bool ray_sphere(Pxf::Math::Vec3f *c, float r, ray_t *ray, intersection_response_t* resp);
bool ray_plane(Pxf::Math::Vec3f *c, Pxf::Math::Vec3f *n, ray_t *ray, intersection_response_t* resp);

#endif /* _INTERSECTIONS_H_ */
