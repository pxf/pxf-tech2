#ifndef _RENDERUTILS_H_
#define _RENDERUTILS_H_

#include <Pxf/Base/Debug.h>
#include <Pxf/Modules/pri/OpenGL.h>
#include "Material.h"
#include "Intersections.h"
#include "RenderUtilDefs.h"

namespace Pxf {
	namespace Resource {
		class Mesh;
	};
};

class Triangle;
class triangle_t;
class BaseLight;

void draw_light(BaseLight* light);
triangle_t* triangle_list(Pxf::Resource::Mesh* mesh);

enum PrimType { UndefinedPrim, SpherePrim, PlanePrim, PointLightPrim, AreaLightPrim, TrianglePrim };

class MaterialLibrary
{
public:
	MaterialLibrary()
	{
	}

	inline void Insert(material_t _Mat,int _Index)
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

struct triangle_t
{
	unsigned material_index;	// 4
	aabb box;					// 24
	vertex_t vertices[3];		// 96
	Pxf::Math::Vec3f n;			// 12
};


class Primitive
{
public:
	Primitive() 
		: material_index(0)
	{	
		
	};

	virtual ~Primitive() { };
	virtual bool Intersects(ray_t* ray,intersection_response_t* resp) = 0;
	virtual PrimType GetType() { return UndefinedPrim; }
	virtual void Draw() { } 

	void SetAABB() { 
		//box = aabb(CalcAABB(*this)); 
	}

	unsigned material_index;	// 4
	aabb box;					// 24
};

class Triangle : public Primitive
{
public:
	Triangle()
		: Primitive()
	{ }

	virtual ~Triangle() { };

	bool Intersects(ray_t* ray,intersection_response_t* resp) { 
		return ray_triangle(vertices[0],vertices[1],vertices[2],ray,resp); 
	}
	PrimType GetType() { return TrianglePrim; }
	void Draw() {
		glBegin(GL_TRIANGLES);
		glVertex3f(vertices[0].v.x,vertices[0].v.y,vertices[0].v.z);
		glVertex3f(vertices[1].v.x,vertices[1].v.y,vertices[1].v.z);
		glVertex3f(vertices[2].v.x,vertices[2].v.y,vertices[2].v.z);
		glEnd();
	}

	vertex_t vertices[3];	// 32
	Pxf::Math::Vec3f n;		// 12
							// 48
};

class BaseLight : public Primitive
{
public:
	BaseLight()
		: Primitive()
	{ }

	Pxf::Math::Vec3f p;
};

class Sphere : public Primitive
{
public:
	//Sphere (Pxf::Math::Vec3f _p, float _r, material_t _material) : Primitive(_material) {p = _p; r = _r;};
	Sphere (Pxf::Math::Vec3f _p, float _r, unsigned _material_index)//material_t* _material) 
		: Primitive()
		, r(_r)
		, p(_p)
		{
			material_index = _material_index; 
		};
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
	Plane (Pxf::Math::Vec3f _p, Pxf::Math::Vec3f _n, unsigned _material_index) {p = _p; n = _n; material_index = _material_index; };
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
	PointLight (Pxf::Math::Vec3f _p, unsigned _material_index) 
		: BaseLight() {p = _p; material_index = _material_index; };
	virtual ~PointLight(){};
	bool Intersects(ray_t *ray, intersection_response_t* resp) { return false; };
	PrimType GetType() { return PointLightPrim; }
};

class AreaLight : public BaseLight//public Primitive
{
public:
	AreaLight (Pxf::Math::Vec3f _p, float _width, float _height, Pxf::Math::Vec3f _n, Pxf::Math::Vec3f _d, float _num_rays, float _strength, unsigned _material_index)
		: width(_width)
		, height(_height)
		, normal(_n)
		, dir(_d)
		, num_rays(_num_rays)
		, strength(_strength)
	{
		p = _p;
		material_index = _material_index;
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