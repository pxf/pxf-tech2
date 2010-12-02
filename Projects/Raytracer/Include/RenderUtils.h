#ifndef _RENDERUTILS_H_
#define _RENDERUTILS_H_

#include <Pxf/Base/Debug.h>
#include "Material.h"
#include "Intersections.h"

namespace Pxf {
	namespace Resource {
		class Mesh;
	};
};

class Triangle;

Triangle* triangle_list(Pxf::Resource::Mesh* mesh);

enum PrimType { SpherePrim, PlanePrim, PointLightPrim, AreaLightPrim, TrianglePrim };

class MaterialLibrary
{
public:
	/*
	MaterialLibrary()
	{
		material_t white_mat;
		m_Materials[0] = 
	} */

	void Insert(material_t _Mat,int _Index)
	{
		PXF_ASSERT(_Index >= 0 && _Index < 64,"Invalid bounds");
		m_Materials[_Index] = _Mat;
	}

	//void Insert(material_t _Mat);
	inline material_t GetMaterial(int _Index)
	{
		PXF_ASSERT(_Index > 0 && _Index < 64,"Invalid bounds");
		return m_Materials[_Index];
	}

private:
	material_t m_Materials[64];
};

/*
class Prim
{
public:
	Prim(){ }
	~Prim() { 
	}

	// create sphere
	Prim(Pxf::Math::Vec3f c,float r)
		: type(SpherePrim)
		, box(0)
	{
		this->c = c;
		this->r = r;

		box = new aabb(CalcAABB(*this));
	}

	// Create triangle
	Prim(Pxf::Math::Vec3f v0,Pxf::Math::Vec3f v1,Pxf::Math::Vec3f v2)
		: type(type = TrianglePrim)
		, box(0)
	{
		v[0] = v0; 
		v[1] = v1;
		v[2] = v2;

		box = new aabb(CalcAABB(*this));
	}

	// Create Plane
	Prim(Pxf::Math::Vec3f p,Pxf::Math::Vec3f n)
		: type(PlanePrim)
		, box(0)
	{
		this->p = p;
		this->n = n;
	}

	// Create PointLight
	Prim(Pxf::Math::Vec3f p)
		: type(PointLightPrim)
		, box(0)
	{
		this->p = p;
	}
	
	// Create AreaLight
	Prim(Pxf::Math::Vec3f p,float w,float h,Pxf::Math::Vec3f n,Pxf::Math::Vec3f d,float num_rays)
		: type(AreaLightPrim)
		, box(0)
		, w(w)
		, h(h)
		, num_rays(num_rays)
	{
		this->p = p;
		this->n = n;
		this->d = d;
	}

	inline int GetType() { return type; }
	inline int GetMaterial() { return material_index; }
	inline void SetMaterial(int index) { material_index = index; } 

	bool Intersect(ray_t *ray, intersection_response_t* resp)
	{
		switch(type)
		{
			case TrianglePrim: return ray_triangle(v,ray,resp); break;
			case SpherePrim: return ray_sphere(&c,r,ray,resp); break;
			case PlanePrim: return ray_plane(&c,&n,ray,resp); break;
			default: return false; break;
		}
	}

	union {
		// sphere
		struct {
			Pxf::Math::Vec3f c;		// p[0]		
			float r;				// p[1].x	
		};

		// triangle
		struct {
			Pxf::Math::Vec3f v[3];
			Pxf::Math::Vec3f n;
		};

		// area light
		struct {
			Pxf::Math::Vec3f p;		// p[0]
			float w;				// p[1].x
			float h;				// p[1].y
			float num_rays;			// p[1].z
			Pxf::Math::Vec3f d;		// p[2]
		};
	};

	int type;
	int material_index;
	aabb* box;
	char padding;
}; */

struct Vertex
{
	Pxf::Math::Vec3f v;		// 12
	Pxf::Math::Vec3f n;		// 12
	Pxf::Math::Vec2f uv;	// 8
};	// 32

class Primitive
{
public:
	Primitive() { };
	virtual ~Primitive() { };
	virtual bool Intersects(ray_t* ray,intersection_response_t* resp) = 0;
	virtual PrimType GetType() = 0;

	material_t* material;
};

class Triangle : public Primitive
{
public:
	//Triangle(Vertex* _Vertices);
	virtual ~Triangle() { };

	bool Intersects(ray_t* ray,intersection_response_t* resp) { return ray_triangle(vertices[0]->v,vertices[1]->v,vertices[2]->v,ray,resp); }
	PrimType GetType() { return TrianglePrim; }

	Vertex*				vertices[3];	// 12
	Pxf::Math::Vec3f	n;		// 12
};

/*
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
	
};

*/

class BaseLight : public Primitive
{
public:
	Pxf::Math::Vec3f p;
};

class Sphere : public Primitive
{
public:
	//Sphere (Pxf::Math::Vec3f _p, float _r, material_t _material) : Primitive(_material) {p = _p; r = _r;};
	Sphere (Pxf::Math::Vec3f _p, float _r, material_t* _material) {p = _p; r = _r; material = _material; };
	virtual ~Sphere(){};
	bool Intersects(ray_t *ray, intersection_response_t* resp) { return ray_sphere(&p, r, ray, resp); };
	PrimType GetType() { return SpherePrim; }
	
	// data
	float r;
	Pxf::Math::Vec3f p;
};

class Plane : public Primitive
{
public:
	//Plane (Pxf::Math::Vec3f _p, Pxf::Math::Vec3f _n, material_t _material) : Primitive(_material) {p = _p; n = _n;};
	Plane (Pxf::Math::Vec3f _p, Pxf::Math::Vec3f _n, material_t* _material) {p = _p; n = _n; material = _material; };
	virtual ~Plane(){};
	bool Intersects(ray_t *ray, intersection_response_t* resp) { return ray_plane(&p, &n, ray, resp); };
	PrimType GetType() { return PlanePrim; }
	
	// data
	Pxf::Math::Vec3f n;
	Pxf::Math::Vec3f p;
};

class PointLight : public BaseLight // public Primitive
{
public:
	PointLight (Pxf::Math::Vec3f _p, material_t* _material) {p = _p; material = _material; };
	virtual ~PointLight(){};
	bool Intersects(ray_t *ray, intersection_response_t* resp) { return false; };
	PrimType GetType() { return PointLightPrim; }
};

class AreaLight : public BaseLight//public Primitive
{
public:
	AreaLight (Pxf::Math::Vec3f _p, float _width, float _height, Pxf::Math::Vec3f _n, Pxf::Math::Vec3f _d, float _num_rays, float _strength, material_t* _material)
		: width(_width)
		, height(_height)
		, normal(_n)
		, dir(_d)
		, num_rays(_num_rays)
		, strength(_strength)
	{
		p = _p;
		material = _material;
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