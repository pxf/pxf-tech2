#include <math.h>
#include <limits.h>
#include <float.h>
#include <stack>

// remove these later
#include <Pxf/Graphics/VertexBuffer.h>
#include <Pxf/Kernel.h>
#include <Pxf/Graphics/GraphicsDevice.h>

#include "BVH.h"
#include "RenderUtils.h"
#include "Intersections.h"

#define KD_EPSILON 1e-4

using namespace Pxf;
using namespace Math;
using namespace std;


// debug stuffs, remove these later
struct debug_sp_t {
	Vec3f min;
	Vec3f max;
	float split_pos;
	unsigned axis;
};

vector<debug_sp_t> g_DebugBuf;
Graphics::VertexBuffer* g_DebugVB = 0;

struct entry_t {
	Vec3f min;
	Vec3f max;
	Vec3f c;

	triangle_t* tri;

	entry_t(Vec3f min,Vec3f max) { this->min = min; this->max = max; }
};

inline Vec3f min_point(Vec3f a,Vec3f b) {
	return Vec3f(min(a.x,b.x),min(a.y,b.y),min(a.z,b.z));
}

inline Vec3f max_point(Vec3f a,Vec3f b) {
	return Vec3f(max(a.x,b.x),max(a.y,b.y),max(a.z,b.z));
}

node_t* recurse(vector<entry_t> work, int depth)
{
	// early stop
	if(work.size() < 4)
	{
		leaf_node_t* leaf = new leaf_node_t;
		for(vector<entry_t>::iterator it=work.begin(); it!=work.end();it++)
			leaf->data.push_back(it->tri);

		return leaf;
	}

	Vec3f _max(-FLT_MAX,-FLT_MAX,-FLT_MAX),_min(FLT_MAX ,FLT_MAX ,FLT_MAX);
	for(int i=0; i < work.size(); i++) {
		entry_t &e = work[i];

		_min = min_point(_min,e.min);
		_max = max_point(_max,e.max);
	}

	Vec3f side = _max - _min;

	// calc no split cost
	int size = work.size();
	float best_split_cost = size * (side.x * side.y + side.y * side.z + side.z * side.x);
	float best_split_pos = FLT_MAX;
	int best_axis = -1;

	// find best split axis
	for(int i=0;i<3;i++)
	{
		int axis = i;
		float start,stop;

		switch(axis) {
		case 0: 
			start = _min.x;
			stop = _max.x;
			break;
		case 1:
			start = _min.y;
			stop = _max.y;
			break;
		case 2:
			start = _min.z;
			stop = _max.z;
			break;
		default: 
			break;
		}

		float l = stop-start;
		if(l < KD_EPSILON) continue;

		float step = l / (1024.0f / (depth + 1));
		for(float split_pos = start+step; split_pos < stop-step; split_pos += step) {
			int count_left = 0,count_right = 0;
			Vec3f left_min(FLT_MAX,FLT_MAX,FLT_MAX),left_max(-FLT_MAX,-FLT_MAX,-FLT_MAX);
			Vec3f right_min(FLT_MAX,FLT_MAX,FLT_MAX),right_max(-FLT_MAX,-FLT_MAX,-FLT_MAX);

			for(int j=0; j<work.size();j++) {
				entry_t e = work[j];

				float v = e.c.GetAxis(axis);
				if(v < split_pos) {
					// assign on left side:
					left_min = min_point(left_min,e.min);
					left_max = max_point(left_max,e.max);
					count_left++;
				} else {
					right_min = min_point(right_min,e.min);
					right_max = max_point(right_max,e.max);
					count_right++;
				}
			}

			if(count_left <= 1 ||count_right <= 1) continue;

			// Calc SAH
			Vec3f left_side = left_max - left_min;
			Vec3f right_side = right_max - right_min;
				
			float left_SAH = left_side.x * left_side.y + left_side.y * left_side.z + left_side.z * left_side.x;
			float right_SAH = right_side.x * right_side.y + right_side.y * right_side.z + right_side.z * right_side.x;
			float total_cost = left_SAH * count_left + right_SAH * count_right;

			if(total_cost < best_split_cost) {
				best_axis = axis;
				best_split_cost = total_cost;
				best_split_pos = split_pos;
			}
		}
	}

	if(best_axis == -1) {
		leaf_node_t* node = new leaf_node_t;
		for(vector<entry_t>::iterator it=work.begin(); it != work.end(); it++)
			node->data.push_back(it->tri);
		return node;
	}

	vector<entry_t> left_entries;
	vector<entry_t> right_entries;
	
	Vec3f left_min(FLT_MAX,FLT_MAX,FLT_MAX),left_max(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	Vec3f right_min(FLT_MAX,FLT_MAX,FLT_MAX),right_max(-FLT_MAX,-FLT_MAX,-FLT_MAX);

	for(int i=0; i < work.size(); i++)
	{
		entry_t e = work[i];
		float v = e.c.GetAxis(best_axis);

		if(v < best_split_pos) {
			left_min = min_point(left_min,e.min);
			left_max = max_point(left_max,e.max);
			left_entries.push_back(e);
		} else {
			right_min = min_point(right_min,e.min);
			right_max = max_point(right_max,e.max);
			right_entries.push_back(e);
		}
	}

	debug_sp_t d_sp;
	d_sp.axis = best_axis;
	d_sp.split_pos = best_split_pos;
	d_sp.min = _min;
	d_sp.max = _max;

	g_DebugBuf.push_back(d_sp);

	inner_node_t* inner_node = new inner_node_t;
	inner_node->left = recurse(left_entries,depth+1);
	inner_node->right = recurse(right_entries,depth+1);
	inner_node->split_pos = best_split_pos;
	inner_node->axis = best_axis;

	return inner_node;
}

int count_nodes(node_t* node)
{
	if(!node->is_leaf()) {
		inner_node_t* n = (inner_node_t*) node;
		return 1 + count_nodes(n->left) + count_nodes(n->right);
	} else 
		return 1;
}

void build_ca_nodes(ca_node_t* ca_nodes, node_t* node, unsigned& c, unsigned& t, int* index_list, triangle_t* start_tri)
{
	unsigned id = c;
	if(!node->is_leaf()) {
		inner_node_t* n = (inner_node_t*) node;
		ca_nodes[id].split_pos = n->split_pos;
		ca_nodes[id].axis = n->axis;
		ca_nodes[id].data.inner_node.left_node = ++c;
		build_ca_nodes(ca_nodes,n->left,c,t,index_list,start_tri);
		ca_nodes[id].data.inner_node.right_node = ++c;
		build_ca_nodes(ca_nodes,n->right,c,t,index_list,start_tri);
	} else {
		leaf_node_t* n = (leaf_node_t*) node;
		ca_nodes[id].data.leaf_node.tri_count = n->data.size();
		ca_nodes[id].data.leaf_node.list_index = t;
		ca_nodes[id].axis = 0x80;
		vector<triangle_t*>::iterator it = n->data.begin();

		//printf("leaf node: c %i i %i\n",ca_nodes[id].data.leaf_node.tri_count,ca_nodes[id].data.leaf_node.list_index);

		for(it; it != n->data.end(); it++) {
			int val = *it - start_tri;
			index_list[t++] = val;
			//printf("  t.index: %i\n",val);
		}
	}
}

void traverse(ca_node_t* start_node, triangle_t* data,ca_node_t* node, int* index_list)
{
	int is_leaf = node->axis & 0x80;
	
	if(!is_leaf) {

		unsigned left = node->data.inner_node.left_node;
		unsigned right = node->data.inner_node.right_node;

		traverse(start_node,data,&start_node[left],index_list);
		traverse(start_node,data,&start_node[right],index_list);

	} else {
		unsigned count = node->data.leaf_node.tri_count;
		unsigned id = node->data.leaf_node.list_index;

		printf("leaf node:\n");
		for(int i = 0; i < count; i++) {
			unsigned index = index_list[id + i];
			triangle_t t = data[index];

			printf("p0: (%.2f,%.2f,%.2f) | ",t.vertices[0].v.x,t.vertices[0].v.y,t.vertices[0].v.z);
			printf("p1: (%.2f,%.2f,%.2f) | ",t.vertices[1].v.x,t.vertices[1].v.y,t.vertices[1].v.z);
			printf("p3: (%.2f,%.2f,%.2f)\n",t.vertices[2].v.x,t.vertices[2].v.y,t.vertices[2].v.z);
		}
	}
}

tree_t* build(triangle_t* data,int num_triangles)
{
	if(!data) return 0;

	vector<entry_t> work;
	Vec3f tmin(FLT_MAX,FLT_MAX,FLT_MAX);
	Vec3f tmax(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	for(int i=0; i < num_triangles; i++) {
		triangle_t tri = data[i];
		entry_t e(Vec3f(FLT_MAX,FLT_MAX,FLT_MAX),Vec3f(-FLT_MAX,-FLT_MAX,-FLT_MAX));
		
		e.min = min_point(e.min,tri.vertices[0].v);
		e.min = min_point(e.min,tri.vertices[1].v);
		e.min = min_point(e.min,tri.vertices[2].v);

		e.max = max_point(e.max,tri.vertices[0].v);
		e.max = max_point(e.max,tri.vertices[1].v);
		e.max = max_point(e.max,tri.vertices[2].v);

		tmin = min_point(e.min,tmin);
		tmax = max_point(e.max,tmax);

		e.c = e.max;
		e.c += e.min;
		e.c *= 0.5;

		e.tri = &data[i];

		work.push_back(e);
	}

	node_t* root = recurse(work,0);
	int box_count = count_nodes(root);

	ca_node_t* node_list = new ca_node_t[box_count]();
	int* index_list = new int[num_triangles]();

	unsigned id = 0;
	unsigned tri_id = 0;
	build_ca_nodes(node_list,root,id,tri_id, index_list,data);

	//traverse(node_list,data,node_list,index_list);

	tree_t* tree = new tree_t;

	tree->min = tmin;
	tree->max = tmax;

	tree->nodes = node_list;
	tree->num_nodes = box_count;
	tree->index_list = index_list;
	tree->triangle_data = data;
	tree->num_triangles = num_triangles;

	tree->stack = new stack_entry_t[box_count];

	Graphics::GraphicsDevice* gfx = Kernel::GetInstance()->GetGraphicsDevice();
	if(g_DebugVB)
		gfx->DestroyVertexBuffer(g_DebugVB);

	g_DebugVB = gfx->CreateVertexBuffer(Graphics::VB_LOCATION_GPU,Graphics::VB_USAGE_STATIC_DRAW);
	g_DebugVB->CreateNewBuffer(g_DebugBuf.size() * 8,sizeof(Vec3f));
	g_DebugVB->SetData(Pxf::Graphics::VB_VERTEX_DATA,0,3);
	g_DebugVB->SetPrimitive(Graphics::VB_PRIMITIVE_LINES);

	vector<debug_sp_t>::iterator it = g_DebugBuf.begin();
	Vec3f* buf = (Vec3f*) g_DebugVB->MapData(Graphics::VB_ACCESS_WRITE_ONLY);
	
	int j = 0;
	for(it;it != g_DebugBuf.end(); it++) {
		debug_sp_t db = *it;
		float sp = db.split_pos;
		Vec3f _min = db.min;
		Vec3f _max = db.max;
		Vec3f p0,p1,p2,p3;

		if(db.axis == 0) {
			p0 = Vec3f(sp,_min.y,_min.z);
			p1 = Vec3f(sp,_max.y,_min.z);
			p2 = Vec3f(sp,_min.y,_max.z);
			p3 = Vec3f(sp,_max.y,_max.z);
		} else if(db.axis == 1) {
			p0 = Vec3f(_min.x,sp,_min.z);
			p1 = Vec3f(_max.x,sp,_min.z);
			p2 = Vec3f(_min.x,sp,_max.z);
			p3 = Vec3f(_max.x,sp,_max.z);
		} else {
			p0 = Vec3f(_min.x,_min.y,sp);
			p1 = Vec3f(_max.x,_min.y,sp);
			p2 = Vec3f(_min.x,_max.y,sp);
			p3 = Vec3f(_max.x,_max.y,sp);
		}

		buf[j] = p0; j++;
		buf[j] = p1; j++;
		buf[j] = p0; j++; 
		buf[j] = p2; j++;
		buf[j] = p2; j++;
		buf[j] = p3; j++;
		buf[j] = p3; j++;
		buf[j] = p1; j++;
	}

	g_DebugVB->UnmapData();

	tree->debug_buffer = g_DebugVB;

	return tree;
}

#define __EPSILON 0.0005

inline bool interval(float a, float b) 
{
	if(a >= (b-__EPSILON) && a <= (b+__EPSILON)) return true;
	else return false;
}

triangle_t* ray_tree_intersection(tree_t* tree, ray_t* ray)
{
	Vec3f minpos = tree->min;
	Vec3f maxpos = tree->max;

	aabb box;
	box.pos = minpos;
	box.size = maxpos - minpos;

	intersection_response_t resp;

	if(!ray_aabb(ray,&box,&resp))
		return 0;

	Vec3f o = ray->o;
	Vec3f d = ray->d;
	Vec3f inv_d = Inverse(d);

	ca_node_t* nodes = tree->nodes;
	ca_node_t* node = nodes;

	triangle_t* ret_p = 0;
	triangle_t* triangle_data = tree->triangle_data;
	int* index_list = tree->index_list;

	// get box extents
	float t_min=-10000.0f,t_max=10000.0f;

	Vec3f _min = (minpos - o) / d;
	Vec3f _max = (maxpos - o) / d;

	for(size_t i=0; i<3; i++)
	{
		float t1 = _min.GetAxis(i);
		float t2 = _max.GetAxis(i);

		if(t1 > t2) 
			std::swap(t1,t2);
		if(t1 > t_min)
			t_min = t1;
		if(t2 < t_max) 
			t_max = t2;
	}

	Vec3f enter = o + d*t_min;
	Vec3f exit = o + d*t_max;

	/*

	stack<stack_node_t,vector<stack_node_t>> candidates;
	int far_node;
	while(node) {

		while(!node->is_leaf()) {
			unsigned char axis = node->axis;
			float split_pos = node->split_pos;

			if(enter.GetAxis(axis) <= split_pos) {
				// left voxel
				if(exit.GetAxis(axis) <= split_pos) {
					// ray hits only left cell
					node = &nodes[node->data.inner_node.left_node];
					continue;
				}

				//if(interval(exit.GetAxis(axis),split_pos)) {
				if(exit.GetAxis(axis) >= split_pos + KD_EPSILON) {
					// ray parallel to split plane, traverse right cell?
					node = &nodes[node->data.inner_node.right_node];
					continue;
				}

				// ray hits both cells
				node = &nodes[node->data.inner_node.left_node];
				far_node = node->data.inner_node.right_node;

			} else {
				// right voxel
				if(exit.GetAxis(axis) > split_pos) {
					// ray hits right cell only
					node = &nodes[node->data.inner_node.right_node];
					continue;
				}

				// ray hits both cells
				node = &nodes[node->data.inner_node.right_node];
				far_node = node->data.inner_node.left_node;
			}
			// distance to split position:
			float t = (split_pos - o.GetAxis(axis)) / d.GetAxis(axis);

			stack_node_t n;
			n.node = far_node;
			n.p = o + d*t;

			candidates.push(n);
		}

		int start_index = node->data.leaf_node.list_index;
		int count = node->data.leaf_node.tri_count;

		float closest_depth = 10000.0f;
		ray_t r = *ray;
		bool retval = false;

		intersection_response_t _resp;

		for(int i=0; i < count; i++) {
			unsigned index = index_list[start_index + i];
			triangle_t t = triangle_data[index];

			if(ray_triangle(t.vertices,&r,&_resp)) {
				retval = true;

				if(resp.depth < closest_depth) {
					ret_p = &triangle_data[index];
					closest_depth = resp.depth;
				}
			}

			if(retval) return ret_p;
		}
		
		if(candidates.empty()) return 0;

		stack_node_t n = candidates.top();
		candidates.pop();

		enter = exit;
		exit = n.p;
		node = &nodes[n.node];
	}


	return 0;
	*/

	// init stack
	stack_entry_t* stack = tree->stack;
	int entry_ptr=0,exit_ptr=1;
	
	Vec3f entry_v = o + d*t_min;
	Vec3f exit_v = o + d*t_max;

	stack[entry_ptr].node = node;
	stack[entry_ptr].p = entry_v;

	stack[exit_ptr].node = 0;
	stack[exit_ptr].p = exit_v;

	ca_node_t* near_node;
	ca_node_t* far_node;

	int i;

	// build stack
	while(node)
	{
		while(!node->is_leaf())
		{
			unsigned axis = node->axis;
			float split_pos = node->split_pos;
			
			// determine near and far nodes 
			if(stack[entry_ptr].p.GetAxis(axis) <= split_pos)
			{
				// enter point is on left side of split_position
				if(stack[exit_ptr].p.GetAxis(axis) <= split_pos)
				{
					// ray hit left child
					node = &nodes[node->data.inner_node.left_node];
					continue;
				}

				// parallel
				if(stack[exit_ptr].p.GetAxis(axis) == split_pos)
				{
					// continue with right child
					node = &nodes[node->data.inner_node.right_node];
					continue;
				}

				// ray hits both children
				far_node = &nodes[node->data.inner_node.right_node];
				node = &nodes[node->data.inner_node.left_node];
			}
			// ray origin is on right child
			else 
			{
				// ray hits right only
				if(stack[exit_ptr].p.GetAxis(axis) > split_pos)
				{
					node = &nodes[node->data.inner_node.right_node];
					continue;
				}

				// ray hits both, in opposite directions
				far_node = &nodes[node->data.inner_node.left_node];
				node = &nodes[node->data.inner_node.right_node];
			}
			
			// get distance to split position
			float t_split = (split_pos - o.GetAxis(axis)) * inv_d.GetAxis(axis); //4 d.GetAxis(axis);

			// set stack pointers
			int ptr = exit_ptr++;
			if(exit_ptr == entry_ptr)
				exit_ptr++;

			stack[exit_ptr].node = far_node;
			stack[exit_ptr].p = o + d * t_split;
			stack[exit_ptr].prev = ptr;
		}

		/*
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
		} */

		int start_index = node->data.leaf_node.list_index;
		int count = node->data.leaf_node.tri_count;

		float closest_depth = 10000.0f;
		ray_t r = *ray;
		bool retval = false;

		intersection_response_t _resp;

		for(int i=0; i < count; i++) {
			unsigned index = index_list[start_index + i];
			triangle_t t = triangle_data[index];

			if(ray_triangle(t.vertices,&r,&_resp)) {
				retval = true;

				if(resp.depth < closest_depth) {
					ret_p = &triangle_data[index];
					closest_depth = resp.depth;
				}
			}

			if(retval) return ret_p;
		}

		entry_ptr = exit_ptr;
		node = stack[exit_ptr].node;
		exit_ptr = stack[entry_ptr].prev;
	}

	return 0;
}


