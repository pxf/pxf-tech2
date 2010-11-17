#ifndef _KDTREE_H_
#define _KDTREE_H_

#include <Pxf/Math/Vector.h>
#include <Intersections.h>

class KDNode;
class KDTree;
class Primitive;
struct aabb;

aabb CalcAABB(Primitive* _Primitives, int _NbrPrim);
void CalcAABB(Primitive* _Primitive,aabb& _Box);
void PrintStatistics(KDTree* t);
bool RayTreeIntersect(KDTree& t,const ray_t& r);

float Min(float a,float b);
float Max(float a,float b);

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


struct split_position {
	float pos;
	bool start_pos;

	int left_count,right_count;
};

class KDNode {
public:
	KDNode()
		: m_RightChild(0)
		, m_LeftChild(0)
		, m_Axis(0)
		, m_PrimitiveData(0)
		, m_IsLeaf(true)
	{ }

	void SetPrimitiveData(Primitive* _Data,unsigned _NbrPrimitives);
	Primitive* GetPrimitiveData() { return m_PrimitiveData; }

	void SetLeftChild(KDNode* _Child) { m_LeftChild = _Child; }
	void SetRightChild(KDNode* _Child) { m_RightChild = _Child; }

	KDNode* GetLeftChild() { return m_LeftChild; }
	KDNode* GetRightChild() { return m_RightChild; }

	void SetSplitPosition(float _SplitPosition) { m_SplitPosition = _SplitPosition; }
	void SetAxis(unsigned _Axis) { m_Axis = _Axis; }

	unsigned GetAxis() { return m_Axis; }
	float GetSplitPos() { return m_SplitPosition; }

	aabb GetAABB() { return m_AABB; }
	void SetAABB(aabb _AABB) { m_AABB = _AABB; }

	void SetIsLeaf(bool _Val) { m_IsLeaf = _Val; }
	bool IsLeaf() { return m_IsLeaf; }
	bool IsEmpty() { return !m_PrimitiveData; }
private:
	KDNode* m_LeftChild;
	KDNode* m_RightChild;

	aabb m_AABB;

	bool m_IsLeaf;
	unsigned m_Axis;
	float m_SplitPosition;

	Primitive* m_PrimitiveData;
};

class KDTree {
public:
	KDTree(unsigned k,unsigned _MaxDepth = 20)
		: m_Root(0)
		, m_Dimensions(k)
		, m_MaxDepth(_MaxDepth)
	{ m_Root = new KDNode(); }

	KDNode* GetRoot() { return m_Root; }
	aabb GetAABB() { return m_AABB; }

	bool Build(Primitive* _PrimitiveData, unsigned _NbrPrimitives);

	struct tree_statistics {
		tree_statistics() { leaves=0; nodes=0; empty_leaves=0; splits=0;}
		unsigned nodes;
		unsigned empty_leaves;
		unsigned leaves;
		unsigned splits;
	};
	tree_statistics GetStats() { return m_Statistics; }

private:
	unsigned m_MaxDepth;
	KDNode* m_Root;
	unsigned m_Dimensions;

	aabb m_AABB;
	tree_statistics m_Statistics;

	void Subdivide(KDNode* _Node, unsigned _NbrPrimitives,aabb _Box,int _Depth);
};

#endif