#include "Intersections.h"
#include <Common.h>
#include <math.h>

using namespace Pxf;
using namespace Math;

bool ray_slab(float min,float max,float raystart, float rayend,float& tbenter,float& tbexit)
{
	float raydir = rayend - raystart;

	// ray parallel to the slab
	if (fabs(raydir) < 1.0E-9f)
	{
		// ray parallel to the slab, but ray not inside the slab planes
		if(raystart < min || raystart > max)
		{
			return false;
		}
		// ray parallel to the slab, but ray inside the slab planes
		else
		{
			return true;
		}
	}

	// slab's enter and exit parameters
	float tsenter = (min - raystart) / raydir;
	float tsexit = (max - raystart) / raydir;

	if(tsenter > tsexit)
		swap(tsenter,tsexit);

	if(tbenter > tsexit || tsenter > tbexit)
		return false;
	else
	{
		tbenter = Max(tbenter, tsenter);
		tbexit = Min(tbexit, tsexit);
		return true;
	}
}

bool ray_aabb(aabb* box,ray_t* ray,intersection_response_t* resp)
{
	Vec3f box_min = box->pos;
	Vec3f box_max = box_min + box->size;

	Vec3f tmin = (box_min - ray->o)/ray->d;
	Vec3f tmax = (box_max - ray->o)/ray->d;

	float tenter	= 0.0f;
	float texit		= 1.0f;
	
	if(!ray_slab(box_min.x,box_max.x,ray->o.x,box_max.x,tenter,texit))
		return false;
	if(!ray_slab(box_min.y,box_max.y,ray->o.y,box_max.y,tenter,texit))
		return false;
	if(!ray_slab(box_min.z,box_max.z,ray->o.z,box_max.z,tenter,texit))
		return false;

	return true;
}

bool ray_triangle(Vec3f* data,ray_t* ray, intersection_response_t* resp)
{
	// STEP 1: Find point in the plane of the triangle
	Vec3f A = data[0];
	Vec3f B = data[1];
	Vec3f C = data[2];
	
	Vec3f AB = B - A;		// v0
	Vec3f AC = C - A;		// v1

	Vec3f N = Cross(AB,AC);
	Normalize(N);

	intersection_response_t r0;
	if(!ray_plane(&A,&N,ray, &r0))
		return false;

	// STEP 2: Check wether or not point p is inside triangle (http://www.blackpawn.com/texts/pointinpoly/default.html)
	Vec3f AP = r0.p - A;	// v2
	float dot00 = Dot(AB,AB);	// dot(v0,v0)
	float dot01 = Dot(AB,AC);	// dot(v0,v1)
	float dot02	= Dot(AB,AP);	// dot(v0,v2)
	float dot11 = Dot(AC,AC);	// dot(v1,v1)
	float dot12 = Dot(AC,AP);	// dot(v1,v2)

	// barycentric coordinates (http://mathworld.wolfram.com/BarycentricCoordinates.html)
	float inv_denom = 1.0f / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * inv_denom;
	float v = (dot00 * dot12 - dot01 * dot02) * inv_denom;

	// bounds check
	return (u > 0.0f) && (v > 0.0f) && (u + v < 1.0f);
}

bool ray_sphere(Pxf::Math::Vec3f *c, float r, ray_t *ray, intersection_response_t* resp)
{
	// algorithm from Real-Time Rendering (3rd Edition), p741 (optimized solution)
	Pxf::Math::Vec3f l = *c - ray->o;
	float s = Dot(l,ray->d);
	float l2 = Dot(l,l);
	float r2 = r * r;
	
	if (s < 0 && l2 > r2)
		return false;
	
	float s2 = s * s;
	float m2 = l2 - s2;
	
	if (m2 > r2)
		return false;
		
	float q = sqrt(r2 - m2);
	
	float t = 0.0f;
	if (l2 > r2)
		t = s - q;
	else
		t = s + q;
	
	resp->p = ray->o + ray->d * t;
	resp->n = resp->p - *c;
	resp->depth = t;
	Normalize(resp->n);
	return true;
}

bool ray_plane(Pxf::Math::Vec3f *c, Pxf::Math::Vec3f *n, ray_t *ray, intersection_response_t* resp)
{
	// algorithm from http://www.softsurfer.com/Archive/algorithm_0104/algorithm_0104B.htm
	Pxf::Math::Vec3f w = ray->o - *c;
	float t = - Dot(*n,w) / (Dot(*n,ray->d));
	
	if (t < 0.0f)
		return false;
	
	resp->depth = t;
	resp->p = ray->o + ray->d * t;
	resp->n = *n;
	Normalize(resp->n);
	// TODO: Return normal for intersection also!!
	return true;
}
