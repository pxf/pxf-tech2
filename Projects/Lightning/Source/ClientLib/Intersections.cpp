#include "Intersections.h"
#include "RenderUtils.h"
#include "RenderUtilDefs.h"

using namespace Pxf;
using namespace Math;

bool ray_aabb(ray_t* ray,aabb* box,intersection_response_t* resp)
{
	float t_min = -100000.0f;	// MIN VAL
	float t_max = 100000.0f;	// MAX VAL
	float t1,t2;

	Vec3f hw = box->size*0.5f;
	Vec3f Ac = box->pos + hw;
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
		else if( (-e -hw.GetAxis(i) > 0) || (-e+hw.GetAxis(i) < 0))
		{
			return false;
		}
	}

	//aabb_intersection_response_t* r = (aabb_intersection_response_t*) resp;
	//r->t_min = t_min;
	//r->t_max = t_max;

	if(t_min < t_max)
		resp->depth = t_min;
	else
		resp->depth = t_max;

	return true;
}

bool ray_triangle(vertex_t* data,ray_t* ray, intersection_response_t* resp)
{
	if(!data)
		return false;

	vertex_t A = data[0];
	vertex_t B = data[1];
	vertex_t C = data[2];

	return ray_triangle(A,B,C,ray,resp);
}

//bool ray_triangle(Pxf::Math::Vec3f A,Pxf::Math::Vec3f B,Pxf::Math::Vec3f C,ray_t* ray, intersection_response_t* resp)
bool ray_triangle(vertex_t A,vertex_t B,vertex_t C,ray_t* ray, intersection_response_t* resp)
{

	// STEP 1: Find point in the plane of the triangle
	Vec3f AB = B.v - A.v;		// v0
	Vec3f AC = C.v - A.v;		// v1

	Vec3f N = Cross(AB,AC);
	Normalize(N);

	if(!ray_plane(&A.v,&N,ray, resp))
		return false;

	// STEP 2: Check wether or not point p is inside triangle (http://www.blackpawn.com/texts/pointinpoly/default.html)
	Vec3f AP = resp->p - A.v;	// v2
	float dot00 = Dot(AB,AB);	// dot(v0,v0)
	float dot01 = Dot(AB,AC);	// dot(v0,v1)
	float dot02	= Dot(AB,AP);	// dot(v0,v2)
	float dot11 = Dot(AC,AC);	// dot(v1,v1)
	float dot12 = Dot(AC,AP);	// dot(v1,v2)

	// barycentric coordinates (http://mathworld.wolfram.com/BarycentricCoordinates.html)
	float inv_denom = 1.0f / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * inv_denom;
	float v = (dot00 * dot12 - dot01 * dot02) * inv_denom;

	// calculate interpolated normal for point p
	N = A.n + (B.n - A.n) * u + (C.n - A.n) * v;
	Normalize(N);

	resp->n = N;

	// bounds check
	return (u > 0.0f) && (v > 0.0f) && (u + v < 1.0f);
}

bool ray_sphere(Pxf::Math::Vec3f *c, float r, ray_t *ray, intersection_response_t* resp)
{
	/*float b = Dot(ray->d, (ray->o - *c));
	float c0 = Dot(ray->o - *c, ray->o - *c) - r*r;
	float d = b*b - c0;
	
	if (d < 0.0f)
		return false;
		
	float t0,t1,t,e = sqrt(d);
	t0 = -b + e;
	t1 = -b - e;
	
	if (t0 < t1)
		t = t0;
	else
		t = t1;
	
	resp->p = ray->o + ray->d * t;
	resp->n = resp->p - *c;
	resp->depth = t;
	Normalize(resp->n);
	return true;*/
	
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
	return true;
}

/* HELPER FUNCTIONS */
aabb CalcAABB(triangle_t* _Primitives, int _NbrPrim)
{
	aabb box;

	if(_Primitives)
	{
		//box = _Primitives[0].box;
	}

	// accumulative additions on the bounding boxes
	for(int i = 1; i < _NbrPrim; i++)
	{
		triangle_t p = _Primitives[i];
		//aabb tBox = p.box;
		//box = box + tBox;
	}

	return box;
}

aabb CalcAABB(triangle_t& t)
{
	//int _Type = _Primitive.GetType();

	Vec3f _Pos,_Max,_Size;

	// find the smallest component on each point's axis

	_Pos = t.vertices[0].v;
	_Max = t.vertices[0].v;

	Vec3f v1 = t.vertices[1].v;
	Vec3f v2 = t.vertices[2].v;

	// find smallest x
	if (v1.x < _Pos.x)
		_Pos.x = v1.x;
	if (v2.x < _Pos.x)
		_Pos.x = v2.x;

	// find smallest y
	if (v1.y < _Pos.y)
		_Pos.y = v1.y;
	if (v2.y < _Pos.y)
		_Pos.y = v2.y;

	// find smallest z
	if (v1.z < _Pos.z)
		_Pos.z = v1.z;
	if (v2.z < _Pos.z)
		_Pos.z = v2.z;

	// find max values
	if (v1.x > _Max.x)
		_Max.x = v1.x;
	if (v2.x > _Max.x)
		_Max.x = v2.x;
	if (v1.y > _Max.y)
		_Max.y = v1.y;
	if (v2.y > _Max.y)
		_Max.y = v2.y;
	if (v1.z > _Max.z)
		_Max.z = v1.z;
	if (v2.z > _Max.z)
		_Max.z = v2.z;

	_Size = _Max - _Pos;

	/*
	else if(_Type == SpherePrim)
	{
		Sphere* s = (Sphere*) &_Primitive;
		float r = s->r;
		float d = r * 2.0f;

		// box extents
		_Size = Vec3f(d,d,d);

		// set smallest corner from center point
		_Pos = s->p - Vec3f(r,r,r);
	}*/

	aabb _Box;
	_Box.pos = _Pos;
	_Box.size = _Size;

	return _Box;
}