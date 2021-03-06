#include "RenderUtils.h"
#include <Pxf/Resource/Mesh.h>
#include <Pxf/Modules/pri/OpenGL.h>
#include <string.h>

using namespace Pxf;
using namespace Resource;
using namespace Math;


void draw_light(BaseLight* light)
{
	int lType = light->GetType();
	//material_t lMat = (*light->material);
	//Vec3f c = lMat.diffuse;
	Vec3f p = light->p;
	//glColor3f(c.r,c.g,c.b);

	if(lType == PointLightPrim)
	{
		glPointSize(10.0f);
		glBegin(GL_POINTS);
		glVertex3f(p.x,p.y,p.z);
		glEnd();
	}
	else if(lType == AreaLightPrim)
	{
		//AreaLight (Pxf::Math::Vec3f _p, float _width, float _height, Pxf::Math::Vec3f _n, Pxf::Math::Vec3f _d, float _num_rays, float _strength, material_t* _material)
		AreaLight* _l = (AreaLight*) light;
		Vec3f d = _l->dir;
		Vec3f n = _l->normal;
		Normalize(n);
		Vec3f pn = p + n * 10.0f;


		glPointSize(10.0f);
		glBegin(GL_POINTS);
		glVertex3f(p.x,p.y,p.z);
		glEnd();

		glBegin(GL_LINES);
		glVertex3f(p.x,p.y,p.z);
		glVertex3f(pn.x,pn.y,pn.z);
		glEnd();
	}
}

void draw_triangle(triangle_t t)
{
	glBegin(GL_TRIANGLES);
	glVertex3f(t.vertices[0].v.x,t.vertices[0].v.y,t.vertices[0].v.z);
	glVertex3f(t.vertices[1].v.x,t.vertices[1].v.y,t.vertices[1].v.z);
	glVertex3f(t.vertices[2].v.x,t.vertices[2].v.y,t.vertices[2].v.z);
	glEnd();
}

triangle_t* merge_meshlist(Mesh** meshes, int* materials, int num_meshes) 
{

	triangle_t* tlist = 0;
	int t_count = 0;

	// determine total size
	for(int i=0; i < num_meshes; i++) {
		t_count += meshes[i]->GetData()->triangle_count;
	}

	size_t tri_size = sizeof(triangle_t);
	size_t read_size = tri_size * t_count;
	tlist = (triangle_t*) malloc(read_size);
	
	int offset = 0;
	for(int i=0; i < num_meshes; i++) {
		Mesh* m = meshes[i];
		triangle_t* t = triangle_list(m,materials[i]);

		size_t bytes_to_read = tri_size * m->GetData()->triangle_count;
		memcpy(tlist + offset,t,bytes_to_read);

		offset += m->GetData()->triangle_count;

		delete [] t;
	}

	return tlist;
}

triangle_t* triangle_list(Mesh* mesh,int mat_index)
{
	Mesh::mesh_descriptor* md = mesh->GetData();

	int vertex_count = md->vertex_count;
	int triangle_count = md->triangle_count;
	const unsigned int* indices = md->indices;

	triangle_t* t_list = new triangle_t[triangle_count]();

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

		t_list[t_index].vertices[v_index].v = vertex; // = *v;
		t_list[t_index].vertices[v_index].n = normal;
		t_list[t_index].vertices[v_index].uv = texcoord;

		t_list[t_index].material_index = mat_index;
	}

	return t_list;
}
