#include <KDTree.h>
#include <algorithm>
#include <Common.h>

using namespace Pxf::Math;

void PrintStatistics(KDTree* t)
{
	KDTree::tree_statistics stats = t->GetStats();
	size_t node_size = stats.nodes * sizeof(KDNode);
	size_t leaves_size = (stats.leaves-stats.empty_leaves) * sizeof(Primitive);

	printf("---- KD Tree Statistics ----\n#nodes: %i\n#leaves: %i\n#empty leaves: %i\n#splits: %i\n#nodes.size: %i\n#leaves.size(non-empty): %i\n#tree.size: %i\n#build time: %i\n", 
			stats.nodes,
			stats.leaves,
			stats.empty_leaves,
			stats.splits,
			node_size,
			leaves_size,
			node_size + leaves_size,
			stats.timer.Interval());
}

Primitive* RayTreeIntersect(KDTree& t,ray_t& r, float dist)
{
	Vec3f o = r.o;
	Vec3f d = r.d;

	KDNode* node = t.GetRoot();
	aabb box = node->GetAABB();

	Vec3f min_pos = box.pos;
	Vec3f max_pos = min_pos + box.size;

	float t_min,t_max;

	Primitive* ret_p = 0;

	aabb_intersection_response_t resp;

	if(!ray_aabb(&r,&box,&resp))
		return 0;

	t_min = 0.0f;
	t_max = dist;

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

	Vec3f inv_d = r.inv_d;

	// init stack
	KDStack* stack = t.GetStack();
	int entry_ptr=0,exit_ptr=1;
	
	stack[entry_ptr].node = node;
	stack[entry_ptr].v = o + d*t_min;

	stack[exit_ptr].node = 0;
	stack[exit_ptr].v = o + d*t_max;

	KDNode* near_node;
	KDNode* far_node;

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
			float t_split = (split_pos - o.GetAxis(axis)) / d.GetAxis(axis);

			// set stack pointers
			int ptr = exit_ptr++;
			if(exit_ptr == entry_ptr)
				exit_ptr++;

			stack[exit_ptr].node = far_node;
			stack[exit_ptr].v = o + d * t_split;
			stack[exit_ptr].prev = ptr;
		}

		Primitive* data = node->GetPrimitiveData();
		bool retval = ray_triangle(data->v,&r, &resp);
		if(retval)
			return data;

		entry_ptr = exit_ptr;
		node = stack[exit_ptr].node;
		exit_ptr = stack[entry_ptr].prev;
	}

	return 0;
}

bool KDTree::Build(Primitive* _PrimitiveData, unsigned _NbrPrimitives)
{
	if (!_PrimitiveData)
	{
		return false;
	}

	m_Statistics.timer.Start();

	m_KDStack = new KDStack[_NbrPrimitives];
	m_Root->SetPrimitiveData(_PrimitiveData,_NbrPrimitives);
	Subdivide(m_Root,_NbrPrimitives,m_Root->GetAABB(),0);

	m_Statistics.timer.Stop();

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

	//printf("New split on axis %i\n",axis);

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

		//printf("spos %f larea %f rarea %f cost %f\n",sp.pos, larea,rarea, cost);
	}

	//printf("-----\n");

	if (no_split_cost < best_cost) 
	{
		m_Statistics.leaves++;

		delete(split_list);
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
			_LeftList[i] = p[i];
		else
			_RightList[i-best_sp.left_count] = p[i];
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

	delete(split_list);

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