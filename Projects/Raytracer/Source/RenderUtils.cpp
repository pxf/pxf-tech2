#include "RenderUtils.h"
#include <Pxf/Resource/Mesh.h>

using namespace Pxf;
using namespace Resource;
using namespace Math;


Triangle* triangle_list(Mesh* mesh)
{
	Triangle* t_list = new Triangle();
	Mesh::mesh_descriptor* md = mesh->GetData();

	/*
	bool has_normals;
	bool has_uvmap;
	int vertex_count;
	int triangle_count;

	const float* vertices;
	const float* normals;
	const float* texcoords;
	const unsigned int* indices;
	*/

	int vertex_count = md->vertex_count;
	int triangle_count = md->triangle_count;
	const unsigned int* indices = md->indices;

	for(int i = 0; i < triangle_count*3; i++)
	{
		unsigned int idx = md->indices[i] * 3;
		unsigned int idy = md->indices[i] * 2;
		
		Math::Vec3f vertex = Math::Vec3f(*(md->vertices+idx+0), *(md->vertices+idx+1), *(md->vertices+idx+2));
		Math::Vec3f normal;
		Math::Vec2f texcoord;

		if (md->has_normals)
			normal = Math::Vec3f(*(md->normals+idx+0), *(md->normals+idx+1), *(md->normals+idx+2));

		if (md->has_uvmap)
			texcoord = Math::Vec2f(*(md->texcoords+idy+0), *(md->texcoords+idy+1));

		int t_index = i / 3;
		int v_index = i % 3;

		//t_list[i / 3].vertices[v_index]->v = vertex;

		// set position
		/*
		ptr[i].vertex = vertex;
		ptr[i].normal = normal;
		ptr[i].texcoord = texcoord; */
	}

	return 0;
}
