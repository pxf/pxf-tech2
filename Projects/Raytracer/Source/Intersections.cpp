#include "Intersections.h"

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
	
	resp->p = ray->o + ray->d * t;
	// TODO: Return normal for intersection also!!
	return true;
}
