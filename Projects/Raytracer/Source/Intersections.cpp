#include "Intersections.h"

bool ray_sphere(Vec3f *c, float r, ray_t *ray, intersection_response_t* resp)
{
	// algorithm from Real-Time Rendering (3rd Edition), p741 (optimized solution)
	Vec3f l = *c - ray.o;
	float s = l.dot(ray.d);
	float l2 = l.dot(l);
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
	
	resp.p = ray.o + ray.d * t;
	// TODO: Return normal for intersection also!!
	return true;
}