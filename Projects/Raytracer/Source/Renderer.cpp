#include <Pxf/Math/Vector.h>

#include "Renderer.h"
#include <Pxf/Math/Math.h>

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
		light_ray.d = datablob->lights[l]->p - (*p);
		float light_distance = Length(light_ray.d);
		Normalize(light_ray.d);
		light_ray.o += light_ray.d*0.01f;
		
		// loop geometry, see if we hit something
		bool in_shadow = false;
		for(int i = 0; i < datablob->prim_count; ++i)
		{
			intersection_response_t tresp;
			if (datablob->primitives[i]->Intersects(&light_ray, &tresp))
			{
				// Check if closer than light
				if (tresp.depth < light_distance)
				{
					in_shadow = true;
					break; // light is blocking!
				}
			}
		}
		
		if (!in_shadow)
		{
			// TODO: add better contributing calculations
			float ndotl = Dot(*n, light_ray.d);
			*res += (datablob->lights[l]->material.diffuse * ndotl) / (float)datablob->light_count;
			//*res = Pxf::Math::Vec3f(1.0f);
		}
	}
	
	return true;
}

bool find_first_intersection(batch_blob_t *datablob, ray_t *ray, Primitive **prim, intersection_response_t *resp)
{
	// Loop geometry
	float closest_depth = 100000000000000.0f;
	bool found = false;
	//Primitive *closest_prim = 0x0;
	intersection_response_t closest_resp;
	
	for(int i = 0; i < datablob->prim_count; ++i)
	{
		// test intersection
		if (datablob->primitives[i]->Intersects(ray, &closest_resp))
		{
			if (closest_depth > closest_resp.depth)
			{
				closest_depth = closest_resp.depth;
				*prim = datablob->primitives[i];
				*resp = closest_resp;
				found = true;
			}
		}
	}
	
	return found;
}

bool calculate_pixel(float x, float y, task_detail_t *task, batch_blob_t *datablob, pixel_data_t *pixel)
{	
	// Clear pixel
	Pxf::Math::Vec3f fpixel(0.0f, 0.0f, 0.0f);
	
	// Center ray
	Vec3f screen_coords(-1.0f + x, 1.0f - y, 0.0f);
	ray_t cray;
	cray.o = Vec3f(0.0f,0.0f,-1.0f);
	//cray.d = screen_coords - cray.o;
	//Normalize(cray.d);
	
	// find closest primitive
	for(int pixel_x = 0; pixel_x < datablob->samples_per_pixel; ++pixel_x)
	{
		for(int pixel_y = 0; pixel_y < datablob->samples_per_pixel; ++pixel_y)
		{
			// Offset ray
			ray_t ray = cray;
			Vec3f new_screen_coords = screen_coords;
			
			// TODO: Make sampling more non-uniform
			new_screen_coords.x += (1.0f / ((float)datablob->pic_w * (float)datablob->samples_per_pixel)) * (float)pixel_x;
			new_screen_coords.y += (1.0f / ((float)datablob->pic_h * (float)datablob->samples_per_pixel)) * (float)pixel_y;
			ray.d = new_screen_coords - cray.o;
			Normalize(ray.d);
			
			// Cast ray and see what we find
			Primitive *closest_prim = 0x0;
			intersection_response_t closest_resp;
			if (find_first_intersection(datablob, &ray, &closest_prim, &closest_resp))
			{
				Pxf::Math::Vec3f light_contrib(0.0f, 0.0f, 0.0f);
				Pxf::Math::Vec3f eye_dir = ray.o - closest_resp.p;
				Normalize(eye_dir);
				if (!calc_light_contrib(&closest_resp.p, &closest_resp.n, &eye_dir, datablob, &light_contrib))
				{
					Pxf::Message("calculate_pixel", "Light calculations failed!");
					return false;
				}
				fpixel += closest_prim->material.ambient + closest_prim->material.diffuse * light_contrib;


			}
		}
	}
	
	fpixel /= datablob->samples_per_pixel * datablob->samples_per_pixel;
	
	fpixel.x = Pxf::Math::Clamp(fpixel.x, 0.0f, 1.0f);
	fpixel.y = Pxf::Math::Clamp(fpixel.y, 0.0f, 1.0f);
	fpixel.z = Pxf::Math::Clamp(fpixel.z, 0.0f, 1.0f);
		
	pixel->r = (char)(fpixel.r * 255.0f);
	pixel->g = (char)(fpixel.g * 255.0f);
	pixel->b = (char)(fpixel.b * 255.0f);	
	
	return true;
}
