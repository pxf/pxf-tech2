#ifndef _KDTREE_H_
#define _KDTREE_H_

#include <Pxf/Math/Vector.h>
#include <Pxf/Base/Timer.h>
#include <Common.h>
#include <Intersections.h>

class KDNode;
class KDTree;
class Primitive;
struct aabb;

void PrintStatistics(KDTree* t);
Primitive* RayTreeIntersect(KDTree& t,ray_t& r,float dist);

struct split_position {
	float pos;
	bool start_pos;
	int left_count,right_count;
};

struct KDStack
{
	KDNode* node;
	Pxf::Math::Vec3f v;
	int next,prev;
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

	~KDNode() 
	{
		if(m_LeftChild)
		{
			delete m_LeftChild; m_LeftChild = 0;
		}

		if(m_RightChild)
		{
			delete m_RightChild; m_RightChild = 0;
		}
	}

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
	KDStack* GetStack() { return m_KDStack; }

	aabb GetAABB() { return m_AABB; }

	bool Build(Primitive* _PrimitiveData, unsigned _NbrPrimitives);

	struct tree_statistics {
		tree_statistics() { leaves=0; nodes=0; empty_leaves=0; splits=0;}
		unsigned nodes;
		unsigned empty_leaves;
		unsigned leaves;
		unsigned splits;
		Pxf::Timer timer; 
	};
	tree_statistics GetStats() { return m_Statistics; }

private:
	KDStack* m_KDStack;
	KDNode* m_Root;
	unsigned m_MaxDepth;
	unsigned m_Dimensions;

	aabb m_AABB;
	tree_statistics m_Statistics;

	void Subdivide(KDNode* _Node, unsigned _NbrPrimitives,aabb _Box,int _Depth);
};

#endif

