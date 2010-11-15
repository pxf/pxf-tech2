#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <Pxf/Base/Debug.h>
#include "Material.h"
#include "Intersections.h"

class Primitive
{
public:
	Primitive(material_t _material) {material = _material;};
	virtual ~Primitive (){};
	virtual bool Intersects(ray_t *ray, intersection_response_t* resp) = 0;
	
	material_t material;
	Pxf::Math::Vec3f p;
private:
	/* data */
};

class Sphere : public Primitive
{
public:
	Sphere (Pxf::Math::Vec3f _p, float _r, material_t _material) : Primitive(_material) {p = _p; r = _r;};
	virtual ~Sphere(){};
	bool Intersects(ray_t *ray, intersection_response_t* resp) { return ray_sphere(&p, r, ray, resp); };
	
	// data
	float r;
};

class PointLight : public Primitive
{
public:
	PointLight (Pxf::Math::Vec3f _p, material_t _material) : Primitive(_material) {p = _p;};
	virtual ~PointLight(){};
	bool Intersects(ray_t *ray, intersection_response_t* resp) { return false; };
	
	// data
};

struct batch_blob_t
{
	char lols[256]; // Something stupid for now..
	Primitive* primitives[256];
	Primitive* lights[256];
	int prim_count;
	int light_count;
	int pic_w, pic_h;
	// TODO: Add textures, kd-tree etc etc.
};

struct task_detail_t
{
	int region[4]; // x0,y0,x1,y1
	int task_id;
	int task_count;
	// TODO: Add camera stuff etc!
};

struct pixel_data_t
{
	char r,g,b;
};

struct render_result_t
{
	pixel_data_t *data;
};

bool render_task(task_detail_t *task, batch_blob_t *datablob, render_result_t *pic);
bool calc_light_contrib(Pxf::Math::Vec3f *p, Pxf::Math::Vec3f *n, Pxf::Math::Vec3f *ed, batch_blob_t *datablob, Pxf::Math::Vec3f *res); // ed = eye direction
bool calculate_pixel(float x, float y, task_detail_t *task, batch_blob_t *datablob, pixel_data_t *pixel);

#endif /* _RENDERER_H_ */