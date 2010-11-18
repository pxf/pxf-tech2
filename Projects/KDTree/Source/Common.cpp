#include <Common.h>

using namespace Pxf;
using namespace Math;

float Min(float a,float b) { return (a < b) ? a : b; }
float Max(float a,float b) { return (a > b) ? a : b; }

void swap(float& a,float& b)
{
	float tmp = b;
	b = a;
	a = tmp;
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