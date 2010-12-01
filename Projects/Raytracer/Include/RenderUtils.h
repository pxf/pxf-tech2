#ifndef _RENDERUTILS_H_
#define _RENDERUTILS_H_

#include <Pxf/Base/Debug.h>
#include "Material.h"
#include "Intersections.h"

enum PrimType { SpherePrim, PlanePrim, PointLightPrim, AreaLightPrim };

class Primitive
{
public:
	Primitive(material_t _material) {material = _material;};
	virtual ~Primitive (){};
	virtual bool Intersects(ray_t *ray, intersection_response_t* resp) = 0;
	
	material_t material;
	Pxf::Math::Vec3f p;
	
	virtual PrimType GetType() = 0;
private:
	/* data */
};

class Sphere : public Primitive
{
public:
	Sphere (Pxf::Math::Vec3f _p, float _r, material_t _material) : Primitive(_material) {p = _p; r = _r;};
	virtual ~Sphere(){};
	bool Intersects(ray_t *ray, intersection_response_t* resp) { return ray_sphere(&p, r, ray, resp); };
	PrimType GetType() { return SpherePrim; }
	
	// data
	float r;
};

class Plane : public Primitive
{
public:
	Plane (Pxf::Math::Vec3f _p, Pxf::Math::Vec3f _n, material_t _material) : Primitive(_material) {p = _p; n = _n;};
	virtual ~Plane(){};
	bool Intersects(ray_t *ray, intersection_response_t* resp) { return ray_plane(&p, &n, ray, resp); };
	PrimType GetType() { return PlanePrim; }
	
	// data
	Pxf::Math::Vec3f n;
};

class PointLight : public Primitive
{
public:
	PointLight (Pxf::Math::Vec3f _p, material_t _material) : Primitive(_material) {p = _p;};
	virtual ~PointLight(){};
	bool Intersects(ray_t *ray, intersection_response_t* resp) { return false; };
	PrimType GetType() { return PointLightPrim; }
	
	// data
};

class AreaLight : public Primitive
{
public:
	AreaLight (Pxf::Math::Vec3f _p, float _width, float _height, Pxf::Math::Vec3f _n, Pxf::Math::Vec3f _d, float _num_rays, float _strength, material_t _material) : Primitive(_material)
	{
		p = _p;
		width = _width;
		height = _height;
		normal = _n;
		dir = _d;
		num_rays = _num_rays;
		strength = _strength;
	};
	virtual ~AreaLight(){};
	bool Intersects(ray_t *ray, intersection_response_t* resp) { return false; };
	PrimType GetType() { return AreaLightPrim; }
	
	// data
	float strength;
	float num_rays;
	float width, height;
	Pxf::Math::Vec3f normal;
	Pxf::Math::Vec3f dir;
};


#endif /* _RENDERUTILS_H_ */