#ifndef _KDTREE_H_
#define _KDTREE_H_

#include <Pxf/Math/Vector.h>
#include <Pxf/Base/Timer.h>
#include <Pxf/Graphics/VertexBuffer.h>
#include <RenderUtils.h>
#include <Intersections.h>
#include <vector>

class KDNode;
class KDTree;
class triangle_t;
struct aabb;

void PrintStatistics(KDTree* t);
void CreateVBFromTree(KDTree* t,Pxf::Graphics::VertexBuffer* vb);
triangle_t* RayTreeIntersect(KDTree& t,ray_t& r,float dist,intersection_response_t& resp);

struct split_position {
	float pos;
	bool start;
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
		, m_PrimData(0)
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

	void SetPrimData(triangle_t* _Data,unsigned _NbrPrims);
	triangle_t* GetPrimData() { return m_PrimData; }

	void SetLeftChild(KDNode* _Child) { m_LeftChild = _Child; }
	void SetRightChild(KDNode* _Child) { m_RightChild = _Child; }

	KDNode* GetLeftChild() { return m_LeftChild; }
	KDNode* GetRightChild() { return m_RightChild; }

	void SetSplitPosition(float _SplitPosition) { m_SplitPosition = _SplitPosition; }
	void SetAxis(unsigned _Axis) { m_Axis = _Axis; }

	unsigned GetAxis() { return m_Axis; }
	float GetSplitPos() { return m_SplitPosition; }
	unsigned GetPrimCount() { return m_PrimCount; }
	void SetPrimCount(unsigned _PrimCount) { m_PrimCount = _PrimCount; } 

	aabb GetAABB() { return m_AABB; }
	void SetAABB(aabb _AABB) { m_AABB = _AABB; }

	void SetIsLeaf(bool _Val) { m_IsLeaf = _Val; }
	bool IsLeaf() { return m_IsLeaf; }
	bool IsEmpty() { return !m_PrimData; }
private:
	KDNode* m_LeftChild;
	KDNode* m_RightChild;

	aabb m_AABB;

	bool m_IsLeaf;
	unsigned m_Axis;
	float m_SplitPosition;
	unsigned m_PrimCount;

	triangle_t* m_PrimData;
};

class KDTree {
public:
	KDTree(unsigned k,unsigned _MaxDepth = 20)
		: m_Root(0)
		, m_KDStack(0)
		, m_Dimensions(k)
		, m_MaxDepth(_MaxDepth)
		, m_Initialized(false)
	{ 
		m_Root = new KDNode(); 
	}

	~KDTree() {
		if(m_KDStack)
		{
			delete [] &m_KDStack;	// DOESNT WORK :(
			m_KDStack = 0;
		}

		//m_SplitBuffer.clear();
	}

	KDNode* GetRoot() { return m_Root; }
	KDStack* GetStack() { return m_KDStack; }

	aabb GetAABB() { return m_AABB; }

	bool Build(triangle_t* _PrimData, unsigned _NbrPrims);
	bool IsValid() { return m_Initialized; }

	struct tree_statistics {
		tree_statistics() { leaves=0; nodes=0; empty_leaves=0; splits=0;}
		unsigned nodes;
		unsigned empty_leaves;
		unsigned leaves;
		unsigned splits;
		Pxf::Timer timer; 
	};
	tree_statistics GetStats() { return m_Statistics; }
	std::vector<Pxf::Math::Vec3f> GetSplitList() { return m_SplitBuffer; }
private:
	KDStack* m_KDStack;
	KDNode* m_Root;
	unsigned m_MaxDepth;
	unsigned m_Dimensions;

	aabb m_AABB;
	tree_statistics m_Statistics;

	void Subdivide(KDNode* _Node, unsigned _NbrPrims,aabb _Box,int _Depth);
	bool m_Initialized;

	std::vector<Pxf::Math::Vec3f> m_SplitBuffer;
};

#endif

