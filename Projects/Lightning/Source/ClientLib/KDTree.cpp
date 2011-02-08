#include <KDTree.h>
#include <algorithm>
#include <Pxf/Base/Debug.h>
#include <stdio.h>

using namespace Pxf::Math;

void CreateVBFromTree(KDTree* t,Pxf::Graphics::VertexBuffer* vb)
{
	PXF_ASSERT(t,"Invalid Tree");
	PXF_ASSERT(vb,"Invalid VB");

	KDNode* node = t->GetRoot();
	
	PXF_ASSERT(node,"Invalid Root");

	std::vector<Vec3f> split_points = t->GetSplitList();

	if(split_points.empty())
		return;

	aabb b = t->GetAABB(); //node->GetAABB();

	Vec3f p = b.pos;
	Vec3f w = b.size,h = b.size,d = b.size;

	w.y = 0; w.z = 0;
	h.x = 0; h.z = 0;
	d.x = 0; d.y = 0;

	vb->CreateNewBuffer(split_points.size(),sizeof(Vec3f));
	vb->SetData(Pxf::Graphics::VB_VERTEX_DATA,0,3);
	vb->SetPrimitive(Pxf::Graphics::VB_PRIMITIVE_LINES);
	Vec3f* buf;
	buf = (Vec3f*)vb->MapData(Pxf::Graphics::VB_ACCESS_WRITE_ONLY);

	std::vector<Pxf::Math::Vec3f>::iterator it = split_points.begin();
	size_t i = 0;

	for(it; it != split_points.end(); it++,i++)
		buf[i] = *it;

	vb->UnmapData();

	split_points.clear();
}

triangle_t* RayTreeIntersect(KDTree& t,ray_t& r, float dist,intersection_response_t& resp)
{
	Vec3f o = r.o;
	Vec3f d = r.d;

	KDNode* node = t.GetRoot();
	aabb box = t.GetAABB(); //node->GetAABB();

	Vec3f min_pos = box.pos;
	Vec3f max_pos = min_pos + box.size;

	float t_min,t_max;

	triangle_t* ret_p = 0;

	if(!ray_aabb(&r,&box,&resp))
		return 0;

	t_min = -dist;
	t_max = dist;

	Vec3f min = (min_pos - o) / d;
	Vec3f max = (max_pos - o) / d;

	for(size_t i=0; i<3; i++)
	{
		float t1 = min.GetAxis(i);
		float t2 = max.GetAxis(i);

		if(t1 > t2) 
			std::swap(t1,t2);
		if(t1 > t_min)
			t_min = t1;
		if(t2 < t_max) 
			t_max = t2;
	}

	/*
	// clip line segments to box
	for(size_t i=0; i<3; i++)
	{
		float d_axis = d.GetAxis(i);
		float p = o.GetAxis(i) + t_max * d_axis;

		if (d_axis < 0)
		{
			if(p < min_pos.GetAxis(i))
				t_max = t_min + (t_max - t_min) * ((o.GetAxis(i) - min_pos.GetAxis(i)) / (o.GetAxis(i) - p));
			if(o.GetAxis(i) > max_pos.GetAxis(i))
				t_min += (t_max - t_min) * ((o.GetAxis(i) - max_pos.GetAxis(i)) / (t_max * d.GetAxis(i)));
		}
		else
		{
			if(p > max_pos.GetAxis(i))
				t_max = t_min + (t_max - t_min) * ((max_pos.GetAxis(i) - o.GetAxis(i)) / (p - o.GetAxis(i)));
			
			if(o.GetAxis(i) < min_pos.GetAxis(i))
				t_min += (t_max - t_min) * ((max_pos.GetAxis(i) - o.GetAxis(i)) / (t_max * d.GetAxis(i)));
		}
	}
	*/

	Vec3f inv_d = Inverse(d);

	// init stack
	KDStack* stack = t.GetStack();
	int entry_ptr=0,exit_ptr=1;
	
	Vec3f entry_v = o + d*t_min;
	Vec3f exit_v = o + d*t_max;

	stack[entry_ptr].node = node;
	stack[entry_ptr].v = entry_v;

	stack[exit_ptr].node = 0;
	stack[exit_ptr].v = exit_v;

	KDNode* near_node;
	KDNode* far_node;

	size_t i;

	// build stack
	while(node)
	{
		while(!node->IsLeaf())
		{
			unsigned axis = node->GetAxis();
			float split_pos = node->GetSplitPos();
			
			// determine near and far nodes 
			if(stack[entry_ptr].v.GetAxis(axis) <= split_pos)
			{
				// enter point is on left side of split_position
				if(stack[exit_ptr].v.GetAxis(axis) <= split_pos)
				{
					// ray hit left child
					node = node->GetLeftChild();
					continue;
				}

				// parallel
				if(stack[exit_ptr].v.GetAxis(axis) == split_pos)
				{
					// continue with right child
					node = node->GetRightChild();
					continue;
				}

				// ray hits both children
				far_node = node->GetRightChild();
				node = node->GetLeftChild();
			}
			// ray origin is on right child
			else 
			{
				// ray hits right only
				if(stack[exit_ptr].v.GetAxis(axis) > split_pos)
				{
					node = node->GetRightChild();
					continue;
				}

				// ray hits both, in opposite directions
				far_node = node->GetLeftChild();
				node = node->GetRightChild();
			}
			
			// get distance to split position
			float t_split = (split_pos - o.GetAxis(axis)) * inv_d.GetAxis(axis); //4 d.GetAxis(axis);

			// set stack pointers
			int ptr = exit_ptr++;
			if(exit_ptr == entry_ptr)
				exit_ptr++;

			stack[exit_ptr].node = far_node;
			stack[exit_ptr].v = o + d * t_split;
			stack[exit_ptr].prev = ptr;
		}

		triangle_t* data = node->GetPrimData();

		if(data)
		{
			int nbr_prims = node->GetPrimCount();

			bool retval = false;

			float closest_depth = 10000.0f;
			ray_t test_r = r;

			for (i=0;i<nbr_prims;i++)
			{	
				//if(data[i]->Intersects(&test_r,&resp))
				if(ray_triangle(data[i].vertices,&test_r,&resp))
				{
					retval = true;
					if(resp.depth < closest_depth)
					{
						ret_p = &data[i];
						closest_depth = resp.depth;
					}
				}
			}

			if(retval)
				return ret_p;
		}

		entry_ptr = exit_ptr;
		node = stack[exit_ptr].node;
		exit_ptr = stack[entry_ptr].prev;
	}

	return 0;
}

bool KDTree::Build(triangle_t* _PrimData, unsigned _NbrPrims)
{
	if (!_PrimData)
	{
		return false;
	}

	m_KDStack = new KDStack[_NbrPrims];
	m_Root->SetPrimData(_PrimData,_NbrPrims);

	m_AABB = CalcAABB(_PrimData,_NbrPrims); 
	Subdivide(m_Root,_NbrPrims,m_AABB,0); // m_Root->GetAABB(),0);

	m_Initialized = true;

	return true;
}

double CalcSurfaceArea(const aabb& _Box)
{
	float w = _Box.size.x; float h = _Box.size.y; float d = _Box.size.z;
	return 2 * (w*d + w*h + d*h);
}

inline bool sort_x(triangle_t a,triangle_t b)
{
	return a.box.pos.x < b.box.pos.x;
}

inline bool sort_y(triangle_t a,triangle_t b)
{
	return a.box.pos.y < b.box.pos.y;
}

inline bool sort_z(triangle_t a,triangle_t b)
{
	return a.box.pos.z < b.box.pos.z;
}

void SortPrims(triangle_t* p, unsigned size,int axis)
{
	switch(axis)
	{
	case 0: std::sort(p,p+size,sort_x); break;
	case 1: std::sort(p,p+size,sort_y); break;
	case 2: std::sort(p,p+size,sort_z); break;
	default: break;
	}
}

std::vector<split_position*>* GetSplitPositions(triangle_t* p,int size,int axis)
{
	//int s = size * 2;
	//split_position* slist = new split_position[s];

	std::vector<split_position*>* list = new std::vector<split_position*>();

	/*
	For each start bounds +1 to the total count for the items on the left of the current splitting plane
	For each end bounds -1 to the count of boxes to the right of the splitting plane
	Find straddle by doing left+right-total
	*/
	size_t i;
	std::vector<split_position*>::iterator it;
	// first find all split positions

	int j = 0;
	for(i=0; i < size; i++)
	{
		aabb b = p[i].box;

		float left_border = b.pos.GetAxis(axis);
		float right_border = left_border + b.size.GetAxis(axis);

		split_position* left = new split_position(); 
		left->pos = left_border; 
		left->start = true;

		split_position* right = new split_position(); 
		right->pos = right_border; 
		right->start = false;
		
		/*
		for(it = list->begin(); it != list->end(); it++,j++)
		{
			if((*it)->pos == left_border || (*it)->pos == right_border)
				it = list->erase(it);
		}*/

		list->push_back(left);
		list->push_back(right);
	}

	it = list->begin();
	int left_count;
	int right_count;
	int straddle;
	split_position* sp=0;
	aabb b;

	// check triangle_t bounds with each split position
	i = 0;
	for(it; it != list->end(); it++,i++)
	{
		left_count = 0;
		right_count = size;
		sp = (*it);

		for(i=0;i<size;i++)
		{
			b = p[i].box;
			if(b.pos.GetAxis(axis) < sp->pos)
			{
				left_count++;
				right_count--;
			}
		}

		straddle = left_count + right_count - size;
		sp->left_count = left_count;
		sp->right_count = right_count;

		if(left_count == 0 || right_count == 0)
		{
			//it = list->erase(it); continue;
		}
	}

	return list;
}

void push_split_points(std::vector<Pxf::Math::Vec3f>* vec,float sp,unsigned axis,aabb box)
{
	Vec3f p0,p1,p2,p3;
	float x = box.pos.x;
	float y = box.pos.y;
	float z = box.pos.z;
	float w = box.size.x;
	float h = box.size.y;
	float d = box.size.z;

	if(axis == 0)
	{
		p0 = Vec3f(sp,y,z);
		p1 = Vec3f(sp,y + h,z);
		p2 = Vec3f(sp,y,z+d);
		p3 = Vec3f(sp,y+h,z+d);
	}
	else if(axis == 1)
	{
		p0 = Vec3f(x,sp,z);
		p1 = Vec3f(x + w,sp,z);
		p2 = Vec3f(x,sp,z+d);
		p3 = Vec3f(x + w,sp,z+d);
	}
	else if(axis == 2)
	{
		p0 = Vec3f(x,y,sp);
		p1 = Vec3f(x+w,y,sp);
		p2 = Vec3f(x,y+h,sp);
		p3 = Vec3f(x+w,y+h,sp);
	}

	vec->push_back(p0);
	vec->push_back(p1);

	vec->push_back(p0);
	vec->push_back(p2);

	vec->push_back(p2);
	vec->push_back(p3);

	vec->push_back(p3);
	vec->push_back(p1);
}

void KDTree::Subdivide(KDNode* _Node, unsigned _NbrPrims,aabb _Box, int _Depth)
{
	// select axis depending on depth and dimension
	int axis = _Depth % m_Dimensions;

	/*
	if (_Box.size.x >= _Box.size.y && _Box.size.x >= _Box.size.z) axis = 0;
	else if (_Box.size.y >= _Box.size.x && _Box.size.y >= _Box.size.z) axis = 1;
	else axis = 2; */

	// sort Prim Data on 'active' axis
	triangle_t* p = _Node->GetPrimData();
	SortPrims(p,_NbrPrims,axis);

	// generate list of possible split positions
	std::vector<split_position*>* split_list = GetSplitPositions(p,_NbrPrims,axis);

	// find optimal split
	int best_position = -1;
	float best_cost = 10000;
	float no_split_cost = _NbrPrims * 1.0f;

	float left_extreme = _Box.pos.GetAxis(axis);
	float right_extreme = left_extreme + _Box.size.GetAxis(axis);

	aabb left_aabb = _Box;
	aabb right_aabb = _Box;

	double inv_sav_volume = 1.0f / CalcSurfaceArea(_Box);

	//printf("New possible split on axis %i\n",axis);

	std::vector<split_position*>::iterator it = split_list->begin();

	//for(size_t i = 0; i < _NbrPrims * 2; i++)
	int i = -1;
	for(it; it != split_list->end(); it++)
	{
		split_position* sp = (*it);
		
		float lw = sp->pos - left_aabb.pos.GetAxis(axis);
		float rw = right_extreme - sp->pos;

		left_aabb.size.SetAxis(axis,lw);
		right_aabb.size.SetAxis(axis,rw);

		double larea = CalcSurfaceArea(left_aabb);
		double rarea = CalcSurfaceArea(right_aabb);

		double lcost = larea * inv_sav_volume * sp->left_count;
		double rcost = rarea * inv_sav_volume * sp->right_count;

		// 0.3 is a fine-tuning value and is taken from this implementation:
		// http://www.devmaster.net/articles/raytracing_series/part7.php
		double cost = 0.3 + lcost + rcost; 
		
		i++;

		if (cost + 0.00001f < best_cost)
		{
			best_cost = cost;
			best_position = i;
		}

		//printf("spos %f larea %f rarea %f cost %f\n",sp->pos, larea,rarea, cost);
	}

	if (no_split_cost < best_cost) 
	{
		delete(split_list);
		return;
	}

	split_position* best_sp = split_list->at(best_position);//split_list[best_position];

	_Node->SetSplitPosition(best_sp->pos);
	_Node->SetAxis(axis);

	KDNode* _LeftChild = new KDNode(); 
	_Node->SetLeftChild(_LeftChild);

	KDNode* _RightChild = new KDNode(); 
	_Node->SetRightChild(_RightChild);

	triangle_t* _LeftList = new triangle_t[best_sp->left_count];// = new Prim[best_sp.left_count];
	triangle_t* _RightList = new triangle_t[best_sp->right_count];// = new Prim[best_sp.right_count];

	// split data into two lists

	int left = 0; int right = 0;
	for(i=0; i<_NbrPrims; i++)
	{
		if(p[i].box.pos.GetAxis(axis) < best_sp->pos)
		{
			_LeftList[left] = p[i];
			left++;
		}
		else
		{
			_RightList[right] = p[i];
			right++;
		}
	}

	/*
	for(i=0; i<best_sp->left_count; i++)
		_LeftList[i] = p[i];

	for(i=best_sp->left_count; i < _NbrPrims; i++)
		_RightList[i-best_sp->left_count] = p[i];
		*/

	/*
	for(i=0;i<_NbrPrims; i++)
	{
		if(i < best_sp->left_count)
			_LeftList[i] = p[i];
		else
			_RightList[i-best_sp->left_count] = p[i];
	}*/

	_LeftChild->SetPrimData(_LeftList,best_sp->left_count);
	_RightChild->SetPrimData(_RightList,best_sp->right_count);

	_Node->SetPrimData(0,0);

	left_aabb = _Box;
	//left_aabb.size.SetAxis(axis,best_sp.pos - left_aabb.pos.GetAxis(axis));
	left_aabb.size.SetAxis(axis,best_sp->pos - _Box.pos.GetAxis(axis));

// 	_LeftChild->SetAABB(left_aabb);

	right_aabb = _Box;
	right_aabb.size.SetAxis(axis,_Box.size.GetAxis(axis) - (best_sp->pos - _Box.pos.GetAxis(axis)));//left_aabb.size.GetAxis(axis));
	right_aabb.pos.SetAxis(axis,best_sp->pos);

//	_RightChild->SetAABB(right_aabb);

	_Node->SetIsLeaf(false);

	delete(split_list);
	split_list = 0;

	push_split_points(&m_SplitBuffer,best_sp->pos,axis,_Box);

	if(_Depth < m_MaxDepth)
	{
		if(best_sp->left_count > 0)	
			Subdivide(_LeftChild, best_sp->left_count,left_aabb,_Depth + 1);
		
		if(best_sp->right_count > 0) 
			Subdivide(_RightChild, best_sp->right_count,right_aabb,_Depth + 1);
	}
}

void KDNode::SetPrimData(triangle_t* _Data,unsigned _NbrPrims)
{
	if (_NbrPrims == 0)
	{
		m_PrimData = 0;
		return;
	}

//	m_AABB = CalcAABB(_Data,_NbrPrims);
	m_PrimData = _Data;
	m_PrimCount = _NbrPrims;
}