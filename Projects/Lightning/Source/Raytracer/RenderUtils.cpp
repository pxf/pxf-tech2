#include "RenderUtils.h"
#include <Pxf/Resource/Mesh.h>

using namespace Pxf;
using namespace Resource;
using namespace Math;


Triangle* triangle_list(Mesh* mesh)
{
	Mesh::mesh_descriptor* md = mesh->GetData();

	int vertex_count = md->vertex_count;
	int triangle_count = md->triangle_count;
	const unsigned int* indices = md->indices;

	Triangle* t_list = new Triangle[triangle_count]();


	// set default material to a white material.. 
	material_t* material_white = new material_t();
	material_white->ambient = Pxf::Math::Vec3f(0.1f, 0.1f, 0.1f);
	material_white->diffuse = Pxf::Math::Vec3f(1.0f, 1.0f, 1.0f);
	material_white->reflectiveness = 0.0f;

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

		Triangle* t = &t_list[i / 3];
		t->material = material_white;

		Vertex* v = new Vertex();
		v->v = vertex;
		v->n = normal;
		v->uv = texcoord;
		t->vertices[v_index] = v;
	}

	return t_list;
}
