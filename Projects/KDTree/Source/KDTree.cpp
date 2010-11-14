#include <KDTree.h>
#include <algorithm>

using namespace Pxf::Math;

float Min(float a,float b) { return (a < b) ? a : b; }
float Max(float a,float b) { return (a > b) ? a : b; }

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

	m_AABB = CalcAABB(_PrimitiveData,_NbrPrimitives);
	m_Root->SetPrimitiveData(_PrimitiveData);

	Subdivide(m_Root,_NbrPrimitives,m_AABB,0);

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
	int best_cost = 10000;

	int no_split_cost = _NbrPrimitives * 1.0f;

	float left_extreme = _Box.pos.GetAxis(axis);
	float right_extreme = left_extreme + _Box.size.GetAxis(axis);

	aabb left_aabb = _Box;
	aabb right_aabb = _Box;

	for(size_t i = 0; i < _NbrPrimitives * 2; i++)
	{
		split_position sp = split_list[i];
		
		float lw = sp.pos - left_aabb.pos.GetAxis(axis);
		float rw = right_extreme - sp.pos;

		left_aabb.size.SetAxis(axis,lw);
		right_aabb.size.SetAxis(axis,rw);

		double larea = CalcSurfaceArea(left_aabb);
		double rarea = CalcSurfaceArea(right_aabb);

		printf("left area %f right area %f\n",larea,rarea);
	}

	KDNode* _Left = new KDNode();
	KDNode* _Right = new KDNode();

	if(_Depth < m_MaxDepth)
	{
		// recursion step
	}
}