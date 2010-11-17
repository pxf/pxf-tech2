#ifndef _COMMON_H_
#define _COMMON_H_

#include <Pxf/Math/Vector.h>

/* Common useful math functions */
float Min(float a,float b);
float Max(float a,float b);
void swap(float& a,float& b);

/* Axis Aligned Bounding Box Defs*/
struct aabb;
struct Primitive;

aabb CalcAABB(Primitive* _Primitives, int _NbrPrim);
void CalcAABB(Primitive* _Primitive,aabb& _Box);

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

/* Primitive Def */
struct Primitive {
public:
	enum {
		TRIANGLE,
		SPHERE
	};

	Primitive() { }
	Primitive(Pxf::Math::Vec3f v0,Pxf::Math::Vec3f v1,Pxf::Math::Vec3f v2)
	{
		m_Type = TRIANGLE;
		v[0] = v0;
		v[1] = v1;
		v[2] = v2;

		CalcAABB(this,m_AABB);
	}

	int GetType() { return m_Type; }
	aabb GetAABB() { return m_AABB; }
	
	Pxf::Math::Vec3f* GetPoints() { return v; }

private:
	union {
		// sphere
		struct {
			float radius;
			Pxf::Math::Vec3f center;
		};

		// triangle
		struct {
			Pxf::Math::Vec3f v[3];
			Pxf::Math::Vec3f n;
		};
	};

	int m_Type;
	aabb m_AABB;
};



#endif // _COMMON_H_
