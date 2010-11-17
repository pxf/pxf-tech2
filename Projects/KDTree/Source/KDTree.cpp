#include <KDTree.h>
#include <algorithm>

using namespace Pxf::Math;

float Min(float a,float b) { return (a < b) ? a : b; }
float Max(float a,float b) { return (a > b) ? a : b; }

void PrintStatistics(KDTree* t)
{
	KDTree::tree_statistics stats = t->GetStats();
	printf("---- KD Tree Statistics ----\n#nodes: %i\n#leaves: %i\n#empty leaves: %i\n#splits: %i\n", stats.nodes,stats.leaves,stats.empty_leaves,stats.splits);
}

bool RayTreeIntersect(KDTree& t,const ray_t& r)
{
	KDNode* root = t.GetRoot();


	return true;
}


aabb CalcAABB(Primitive* _Primitives, int _NbrPrim)
{
	aabb box;

	if(_Primitives)
	{
		box = _Primitives[0].GetAABB();
	}

	for(int i = 1; i < _NbrPrim; i++)
	{
		aabb tBox = _Primitives[i].GetAABB();
		box = box + tBox;
	}

	return box;
}

void CalcAABB(Primitive* _Primitive,aabb& _Box)
{
	int _Type = _Primitive->GetType();

	Vec3f _Pos,_Max,_Size;

	if (_Type == Primitive::TRIANGLE)
	{
		_Pos = _Primitive->v[0];
		_Max = _Primitive->v[0];

		Vec3f v1 = _Primitive->v[1];
		Vec3f v2 = _Primitive->v[2];

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
	}
	else if(_Type == Primitive::SPHERE)
	{

	}

	_Box.pos = _Pos;
	_Box.size = _Size;
}

bool KDTree::Build(Primitive* _PrimitiveData, unsigned _NbrPrimitives)
{
	if (!_PrimitiveData)
	{
		return false;
	}

	m_Root->SetPrimitiveData(_PrimitiveData,_NbrPrimitives);

	Subdivide(m_Root,_NbrPrimitives,m_Root->GetAABB(),0);

	return true;
}

double CalcSurfaceArea(const aabb& _Box)
{
	float w = _Box.size.x; float h = _Box.size.y; float d = _Box.size.z;
	return 2 * (w*d + w*h + d*h);
}

bool sort_x(Primitive a,Primitive b)
{
	aabb ab = a.GetAABB();
	aabb bb = b.GetAABB();

	return ab.pos.x < bb.pos.x;
}

bool sort_y(Primitive a,Primitive b)
{
	aabb ab = a.GetAABB();
	aabb bb = b.GetAABB();

	return ab.pos.y < bb.pos.y;
}

bool sort_z(Primitive a,Primitive b)
{
	aabb ab = a.GetAABB();
	aabb bb = b.GetAABB();

	return ab.pos.z < bb.pos.z;
}

void SortPrimitives(Primitive* p, unsigned size,int axis)
{
	if (axis == 0)
		std::sort(p,p+size,sort_x);
	else if(axis == 1)
		std::sort(p,p+size,sort_y);
	else if(axis == 2)
		std::sort(p,p+size,sort_z);
}

split_position* GetSplitPositions(Primitive* p,int size,int axis)
{
	split_position* slist = new split_position[size * 2];

	for(size_t i = 0; i < size; i++)
	{
		aabb b = p[i].GetAABB(); 
		split_position* left = &slist[i*2];
		split_position* right = &slist[i*2 + 1];

		left->pos = b.pos.GetAxis(axis);
		right->pos = b.pos.GetAxis(axis) + b.size.GetAxis(axis);

		left->left_count = i;
		left->right_count = size - i;

		right->left_count = i + 1;
		right->right_count = size - i - 1;
	}

	return slist;
}

void FindOptimalSplitPos(split_position* split_list)
{
	int best_position = -1;
	int best_cost = 10000;
}

void KDTree::Subdivide(KDNode* _Node, unsigned _NbrPrimitives,aabb _Box, int _Depth)
{
	// select axis depending on depth and dimension
	int axis = _Depth % m_Dimensions;

	// sort Primitive Data on 'active' axis
	Primitive* p = _Node->GetPrimitiveData();
	SortPrimitives(p,_NbrPrimitives,axis);

	// generate list of possible split positions
	split_position* split_list = GetSplitPositions(p,_NbrPrimitives,axis);

	// find optimal split
	int best_position = -1;
	float best_cost = 10000;
	float no_split_cost = _NbrPrimitives * 1.0f;

	float left_extreme = _Box.pos.GetAxis(axis);
	float right_extreme = left_extreme + _Box.size.GetAxis(axis);

	aabb left_aabb = _Box;
	aabb right_aabb = _Box;

	double inv_sav_volume = 1.0f / CalcSurfaceArea(_Box);

	printf("New split on axis %i\n",axis);

	for(size_t i = 0; i < _NbrPrimitives * 2; i++)
	{
		split_position sp = split_list[i];
		
		float lw = sp.pos - left_aabb.pos.GetAxis(axis);
		float rw = right_extreme - sp.pos;

		left_aabb.size.SetAxis(axis,lw);
		right_aabb.size.SetAxis(axis,rw);

		double larea = CalcSurfaceArea(left_aabb);
		double rarea = CalcSurfaceArea(right_aabb);

		double lcost = larea * inv_sav_volume * sp.left_count;
		double rcost = rarea * inv_sav_volume * sp.right_count;

		// 0.3 is a fine-tuning value and is taken from this implementation:
		// http://www.devmaster.net/articles/raytracing_series/part7.php
		double cost = 0.3 + lcost + rcost; 

		if (cost < best_cost)
		{
			best_cost = cost;
			best_position = i;
		}

		printf("spos %f larea %f rarea %f cost %f\n",sp.pos, larea,rarea, cost);
	}

	printf("-----\n");

	if (no_split_cost < best_cost) 
	{
		m_Statistics.leaves++;
		return;
	}

	m_Statistics.splits++;

	split_position best_sp = split_list[best_position];

	_Node->SetSplitPosition(best_sp.pos);
	_Node->SetAxis(axis);

	KDNode* _LeftChild = new KDNode(); 
	_Node->SetLeftChild(_LeftChild);

	KDNode* _RightChild = new KDNode(); 
	_Node->SetRightChild(_RightChild);

	m_Statistics.nodes = m_Statistics.nodes + 2;

	Primitive* _LeftList = new Primitive[best_sp.left_count];
	Primitive* _RightList = new Primitive[best_sp.right_count];

	size_t i;

	// split data into two lists
	for(i=0;i<_NbrPrimitives; i++)
	{
		if(i < best_sp.left_count)
			_LeftList[i] = _Node->GetPrimitiveData()[i];
		else
			_RightList[i-best_sp.left_count] = _Node->GetPrimitiveData()[i];
	}

	_LeftChild->SetPrimitiveData(_LeftList,best_sp.left_count);
	_RightChild->SetPrimitiveData(_RightList,best_sp.right_count);

	_Node->SetPrimitiveData(0,0);

	left_aabb = _Box;
	left_aabb.size.SetAxis(axis,best_sp.pos - left_aabb.pos.GetAxis(axis));

 	_LeftChild->SetAABB(left_aabb);

	right_aabb = _Box;
	right_aabb.size.SetAxis(axis,right_aabb.size.GetAxis(axis) - left_aabb.size.GetAxis(axis));
	right_aabb.pos.SetAxis(axis,best_sp.pos);

	_RightChild->SetAABB(right_aabb);

	_Node->SetIsLeaf(false);

	if(_Depth < m_MaxDepth)
	{
		if(best_sp.left_count > 0)	
			Subdivide(_LeftChild, best_sp.left_count,_LeftChild->GetAABB(),_Depth + 1);
		else
		{
			if(_LeftChild->IsEmpty())
				m_Statistics.empty_leaves++;
			m_Statistics.leaves++;
		}
		
		if(best_sp.right_count > 0) 
			Subdivide(_RightChild, best_sp.right_count,_RightChild->GetAABB(),_Depth + 1);
		else
		{
			if(_RightChild->IsEmpty())
				m_Statistics.empty_leaves++;
			m_Statistics.leaves++;
		}
	}
}

void KDNode::SetPrimitiveData(Primitive* _Data,unsigned _NbrPrimitives)
{
	if (_NbrPrimitives == 0)
	{
		m_PrimitiveData = 0;
		return;
	}

	m_AABB = CalcAABB(_Data,_NbrPrimitives);
	m_PrimitiveData = _Data;
}