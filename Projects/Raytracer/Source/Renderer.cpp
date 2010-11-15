#include <Pxf/Math/Vector.h>

#include "Renderer.h"

using namespace Pxf;
using namespace Math;

/**
 * Renders a task, using region etc and storing the data in pic.
 * Returns true if the task was successfully rendered, false otherwise.
 */
bool render_task(task_detail_t *task, batch_blob_t *datablob, render_result_t *pic)
{
	// TODO: Check incomming data!
	
	if (task->region[0] < 0 || task->region[1] < 0 || task->region[2] < 0 || task->region[3] < 0 ||
	    task->region[0] >= task->region[2] || task->region[1] >= task->region[3]
	    // TODO: See if it is outside the "window"
		 )
	{
		Pxf::Message("TaskRenderer", "Task region invalid! [x0: %d, y0: %d, x1: %d, y1: %d]", task->region[0], task->region[1], task->region[2], task->region[3]);
		return false;
	}
	
	// Init data
	int region_width = task->region[2] - task->region[0];
	int region_height = task->region[3] - task->region[1];
	float pixel_w = 1.0f / (datablob->pic_w / 2.0f);
	float pixel_h = 1.0f / (datablob->pic_h / 2.0f);
	pic->data = new pixel_data_t[region_width*region_height]; // width * height
	
	// Loop through all region pixels
	for(int y = 0; y < region_height; ++y)
	{
		for(int x = 0; x < region_width; ++x)
		{
			// Calculate pixel value
			float xf, yf;
			xf = (task->region[0] + x)*pixel_w;
			yf = (task->region[1] + y)*pixel_h;
			if (!calculate_pixel(xf, yf, task, datablob, &pic->data[y*region_width+x]))
			{
				Pxf::Message("TaskRenderer", "Error while calculating pixel: [x: %d, y: %d]", x, y);
				return false;
			}
		}
	}
	
	// Return result
	return true;
}

bool calc_light_contrib(Pxf::Math::Vec3f *p, Pxf::Math::Vec3f *n, Pxf::Math::Vec3f *ed, batch_blob_t *datablob, Pxf::Math::Vec3f *res)
{
	// loop lights
	for(int l = 0; l < datablob->light_count; ++l)
	{
		// create ray to light
		ray_t light_ray;
		light_ray.o = *p;
		light_ray.d = datablob->lights[l]->p - *p;
		Normalize(light_ray.d);
		
		// loop geometry, see if we hit something
		bool in_shadow = false;
		for(int i = 0; i < datablob->prim_count; ++i)
		{
			intersection_response_t tresp;
			if (datablob->primitives[i]->Intersects(&light_ray, &tresp))
			{
				in_shadow = true;
				break; // light is blocking!
			}
		}
		
		if (!in_shadow)
		{
			// TODO: add better contributing calculations
			//float ndotl = Dot(*n, light_ray.d);
			//*res += (datablob->lights[l]->material.diffuse * ndotl);// / (float)datablob->light_count;
			*res = Pxf::Math::Vec3f(1.0f);
		}
	}
	
	return true;
}

bool calculate_pixel(float x, float y, task_detail_t *task, batch_blob_t *datablob, pixel_data_t *pixel)
{	
	// Clear pixel
	Pxf::Math::Vec3f fpixel(0.0f, 0.0f, 0.0f);
	pixel->r = 0;
	pixel->g = 0;
	pixel->b = 0;
	
	// First ray (TODO: sampling!)
	Vec3f screen_coords(-1.0f + x, -1.0f + y, 0.0f);
	ray_t ray;
	ray.o = Vec3f(0.0f,0.0f,-1.0f);
	ray.d = screen_coords - ray.o;
	Normalize(ray.d);
	
	intersection_response_t resp;
	
	// Loop geometry
	float closest_depth = -1.0f;
	Primitive *closest_prim = 0x0;
	intersection_response_t closest_resp;
	
	for(int i = 0; i < datablob->prim_count; ++i)
	{
		// test intersection
		if (datablob->primitives[i]->Intersects(&ray, &resp))
		{
			if (closest_depth < 0.0f || closest_depth > resp.depth)
			{
				closest_depth = resp.depth;
				closest_prim = datablob->primitives[i];
				closest_resp = resp;
			}
		}
	}
	
	// if we found 
	if (closest_prim)
	{
		
		Pxf::Math::Vec3f light_contrib(0.0f, 0.0f, 0.0f);
		Pxf::Math::Vec3f eye_dir = ray.o - resp.p;
		Normalize(eye_dir);
		if (!calc_light_contrib(&resp.p, &resp.n, &eye_dir, datablob, &light_contrib))
		{
			Pxf::Message("calculate_pixel", "Light calculations failed!");
			return false;
		}
		
		fpixel = closest_prim->material.diffuse * light_contrib;
		
		pixel->r = (char)(fpixel.r * 255.0f);
		pixel->g = (char)(fpixel.g * 255.0f);
		pixel->b = (char)(fpixel.b * 255.0f);
		
	}
	
	/*
	pixel->r = 0;//(char)(255 * x / 2.0f);//(char)(255 * ((float)x / (float)region_width));
	pixel->g = 0;//(char)(255 * y / 2.0f);//(char)(255 * ((float)y / (float)region_height));
	pixel->b = 0;//(char)(255 * ((float)task->task_id / (float)task->task_count));
	
	// test sphere
	Vec3f sphere_c(0.0f,0.0f,10.0f);
	ray_t ray;
	ray.o = Vec3f(0.0f,0.0f,-1.0f);
	ray.d = screen_coords - ray.o;
	Normalize(ray.d);
	intersection_response_t resp;
	
	// test light
	Vec3f light(9.0f, 9.0f, -9.0f);
	
	if (ray_sphere(&sphere_c, 7, &ray, &resp))
	{
		Vec3f L = light - resp.p;
		Normalize(L);
		float dot = Dot( resp.n, L );
		if (dot < 0.0f)
			dot = 0.0f;
		else if (dot > 1.0f)
			dot = 1.0f;
		
		resp.n = (resp.n + 1.0f) / 2.0f;
		pixel->r = dot * 255;//resp.n.x * 255;
		pixel->g = dot * 255;//resp.n.y * 255;
		pixel->b = dot * 255;//resp.n.z * 255;
	}*/
	
	return true;
}
