#include "Intersections.h"
#include "Common.h"
#include <math.h>

using namespace Pxf;
using namespace Math;

bool ray_aabb(ray_t* ray,aabb* box,intersection_response_t* resp)
{
	float t_min = -100000.0f;	// MIN VAL
	float t_max = 100000.0f;	// MAX VAL
	float t1,t2;

	Vec3f Ac = box->pos + box->size*0.5;
	Vec3f p = Ac - ray->o;
	Vec3f d = ray->d;
	float f,e;

	Vec3f min = (box->pos - ray->o) / d;
	Vec3f max = (box->pos + box->size - ray->o) / d;

	for(size_t i=0;i<3;i++)
	{
		e = p.GetAxis(i);
		f = d.GetAxis(i);

		if(fabs(f) > 1.0E-9f)
		{
			float t1 = min.GetAxis(i);
			float t2 = max.GetAxis(i);

			if(t1 > t2) 
				std::swap(t1,t2);
			if(t1 > t_min)
				t_min = t1;
			if(t2 < t_max) 
				t_max = t2;
			if(t_min > t_max) 
				return false;
			if(t_max < 0)
				return false;
		}
		else if( (-e -p.GetAxis(i) > 0) || (-e+p.GetAxis(i) < 0))
		{
			return false;
		}
	}

	aabb_intersection_response_t* r = (aabb_intersection_response_t*) resp;
	r->t_min = t_min;
	r->t_max = t_max;

	return true;
}

bool ray_triangle(Vec3f* data,ray_t* ray, intersection_response_t* resp)
{
	if(!data)
		return false;

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

