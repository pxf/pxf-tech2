#ifndef _BVH_H_
#define _BVH_H_

#include <vector>
#include <Pxf/Math/Vector.h>

namespace Pxf {
	namespace Graphics {
		class VertexBuffer;
	}
}

struct intersection_response_t;
struct node_t;
struct triangle_t;
struct tree_t;
struct ray_t;
struct aabb_t;

tree_t* build(triangle_t* data,int num_triangles);
triangle_t* ray_tree_intersection(tree_t* tree, ray_t* ray, intersection_response_t &resp);
triangle_t* ray_tree_find_occluder(tree_t* tree, ray_t* ray, intersection_response_t &resp, float dist);

tree_t* load_BVH(const char* path);
void write_BVH(const char* path,tree_t* t);

struct aabb_t {
	Pxf::Math::Vec3f min;
	Pxf::Math::Vec3f max;
};

struct node_t
{
	virtual bool is_leaf() = 0;
};

struct inner_node_t : node_t
{
	char axis;
	float split_pos;

	node_t* left;
	node_t* right;

	bool is_leaf() { return false; }
};

struct leaf_node_t : node_t
{
	std::vector<triangle_t*> data;
	bool is_leaf() { return true; }
};

// cache aligned node
struct ca_node_t
{
	unsigned char axis;			// set top bit if leaf 1
	float split_pos;			// 4

	union {
		struct {
			unsigned left_node;
			unsigned right_node;
		} inner_node;

		struct {
			unsigned tri_count; 
			unsigned list_index;
		} leaf_node;
	} data; // 8

	bool is_leaf() { return axis & 0x80; }
};

struct stack_node_t {
	int node;
	Pxf::Math::Vec3f p;
};

struct stack_entry_t {
	int next;
	int prev;
	Pxf::Math::Vec3f p;
	ca_node_t* node;
	int node_index;
};

struct tree_t 
{
	// bounding box
	Pxf::Math::Vec3f min;
	Pxf::Math::Vec3f max;
	// data
	ca_node_t* nodes;
	int num_nodes;
	int num_triangles;
	int num_triangles_data;
	int* index_list;
	triangle_t* triangle_data;

	Pxf::Graphics::VertexBuffer* debug_buffer;

	// stack
	stack_entry_t* stack;
};

#endif /* _BVH_H_ */
