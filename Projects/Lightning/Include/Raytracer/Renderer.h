#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "RenderUtils.h"
#include "KDTree.h"
#include "Camera.h"

struct batch_blob_t
{	
	// scene data
	Primitive* primitives[256];
	Primitive* lights[256];
	int prim_count;
	int light_count;
	
	// sampling data
	float samples[256];
	
	// bounces
	int bounce_count;
	
	// windowing etc
	int pic_w, pic_h;
	int samples_per_pixel;
	
	// "interleaved feedback"
	int interleaved_feedback;
	
	// TODO: Add textures, kd-tree etc etc.
	/*KDTree* tree;
	Camera* cam;*/
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

int render_task(task_detail_t *task, batch_blob_t *datablob, render_result_t *pic, int sub_task_num);
bool calc_light_contrib(Pxf::Math::Vec3f *p, Pxf::Math::Vec3f *n, Pxf::Math::Vec3f *ed, batch_blob_t *datablob, Pxf::Math::Vec3f *res); // ed = eye direction
bool calc_ray_contrib(ray_t *ray, batch_blob_t *datablob, Pxf::Math::Vec3f *res, int bounce);
bool calculate_pixel(float x, float y, task_detail_t *task, batch_blob_t *datablob, pixel_data_t *pixel);

#endif /* _RENDERER_H_ */