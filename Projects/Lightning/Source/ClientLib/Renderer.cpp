#include <Pxf/Math/Vector.h>

#include "Renderer.h"
#include <Pxf/Math/Math.h>
#include <cstdlib>

using namespace Pxf;
using namespace Math;

/**
 * Renders a task, using region etc and storing the data in pic.
 * Returns true if the task was successfully rendered, false otherwise.
 */
int render_task(task_detail_t *task, batch_blob_t *datablob, render_result_t *pic, int sub_task_num)
{
	// TODO: Check incomming data!
	
	if (task->region[0] < 0 || task->region[1] < 0 || task->region[2] < 0 || task->region[3] < 0 ||
	    task->region[0] >= task->region[2] || task->region[1] >= task->region[3]
	    // TODO: See if it is outside the "window"
		 )
	{
		Pxf::Message("TaskRenderer", "Task region invalid! [x0: %d, y0: %d, x1: %d, y1: %d]", task->region[0], task->region[1], task->region[2], task->region[3]);
		return -1;
	}
	
	// Init data
	int region_width = task->region[2] - task->region[0];
	int region_height = task->region[3] - task->region[1];
	float pixel_w = 1.0f / (datablob->pic_w / 2.0f);
	float pixel_h = 1.0f / (datablob->pic_h / 2.0f);
	
	// only allocate data for the first sub task
	if (sub_task_num == 0)
		pic->data = new pixel_data_t[region_width*region_height]; // width * height
	
	// Calculate interleave offsets
	int x_off, y_off;
	x_off = sub_task_num % datablob->interleaved_feedback;
	y_off = sub_task_num / datablob->interleaved_feedback;
	
	// Loop through all region pixels
	for(int y = y_off; y < region_height; y += datablob->interleaved_feedback)
	{
		for(int x = x_off; x < region_width; x += datablob->interleaved_feedback)
		{
			// Calculate pixel value
			float xf, yf;
			xf = (task->region[0] + x)*pixel_w;
			yf = (task->region[1] + y)*pixel_h;
			if (!calculate_pixel(xf, yf, task, datablob, &pic->data[y*region_width+x]))
			{
				Pxf::Message("TaskRenderer", "Error while calculating pixel: [x: %d, y: %d]", x, y);
				return -1;
			}
			
			if (sub_task_num == 0)
			{
				for(int yi = 0; yi < datablob->interleaved_feedback; ++yi)
				{
					for(size_t xi = 0; xi < datablob->interleaved_feedback; ++xi)
					{
						pic->data[(y+yi)*region_width+(x+xi)] = pic->data[y*region_width+x];
					}
				}
			}
		}
	}
	
	// Return result
	return datablob->interleaved_feedback*datablob->interleaved_feedback - (sub_task_num+1);
}

// best function name ever
bool find_any_intersection_closer_than(batch_blob_t *datablob, ray_t *ray, float max_distance, intersection_response_t *resp)
{
	// Loop geometry
	//float closest_depth = 100000000000000.0f;
	//bool found = false;
	//Primitive *closest_prim = 0x0;
	intersection_response_t closest_resp;


	if(ray_tree_find_occluder(datablob->tree, ray,closest_resp,max_distance)) {
		if(max_distance > closest_resp.depth) {
			*resp = closest_resp;
			return true;
		}
	}

	/*
	if(ray_tree_intersection(datablob->tree, ray, closest_resp)) {
		if(max_distance > closest_resp.depth) {
			*resp = closest_resp;
			return true;
		}
	}*/

	/*
	for(int i = 0; i < datablob->prim_count; ++i)
	{
		// test intersection
		//if (datablob->primitives[i]->Intersects(ray, &closest_resp))

		if(ray_triangle(datablob->primitives[i].vertices,ray,&closest_resp))
		{
			if (max_distance > closest_resp.depth)
			{
				*resp = closest_resp;
				return true;
			}
		}
	}*/
	
	return false;
}

bool find_intersection(batch_blob_t *datablob, ray_t *ray, triangle_t **prim, intersection_response_t *resp)
{
	// Loop geometry
	float closest_depth = 100000000000000.0f;
	bool found = false;
	//Primitive *closest_prim = 0x0;
	intersection_response_t closest_resp;
	
	if(!datablob->tree) return false;

	triangle_t* p = ray_tree_intersection(datablob->tree, ray, closest_resp); //RayTreeIntersect(*datablob->tree,*ray,10000.0f,closest_resp);

	if(p) 
	{
		//closest_depth = closest_resp.depth;
		*prim = p;
		*resp = closest_resp;
		return true;
	}

	/*
	for(int i = 0; i < datablob->prim_count; ++i)
	{
		// test intersection

		//if (datablob->primitives[i]->Intersects(ray, &closest_resp))
		if(ray_triangle(datablob->primitives[i].vertices,ray,&closest_resp))
		{
			if (closest_depth > closest_resp.depth)
			{
				closest_depth = closest_resp.depth;
				*prim = &datablob->primitives[i];
				*resp = closest_resp;
				found = true;
			}
		}
	} */
	
	return found;
}

bool calc_multisample_ray(ray_t *primray, batch_blob_t *datablob, Pxf::Math::Vec3f *res, float spread, int bounce)
{
	Pxf::Math::Vec3f fsubres(0.0f,0.0f,0.0f);
	// find closest primitive
	for(int sample_x = 0; sample_x < datablob->samples_per_pixel / (bounce+1); ++sample_x)
	{
		for(int sample_y = 0; sample_y < datablob->samples_per_pixel / (bounce+1); ++sample_y)
		{
			// Offset ray
			ray_t ray = *primray;
			Vec3f offset = Vec3f((0.5f * spread) * (datablob->samples[rand() % 255] * 2.0f - 1.0f),
			                     (0.5f * spread) * (datablob->samples[rand() % 255] * 2.0f - 1.0f),
			                     (0.5f * spread) * (datablob->samples[rand() % 255] * 2.0f - 1.0f));
			
			ray.d += offset;
			Normalize(ray.d);
			
			// Calc direct light
			Pxf::Math::Vec3f light_contrib;
			if (!calc_ray_contrib(&ray, datablob, &light_contrib, bounce+1))
			{
				Pxf::Message("calculate_pixel", "Light calculations failed!");
				return false;
			}
			if(light_contrib.x > 0.0f && light_contrib.y > 0.0f && light_contrib.z > 0.0f)
				fsubres += light_contrib;
		}
	}
	
	fsubres /= (datablob->samples_per_pixel / (bounce+1)) * (datablob->samples_per_pixel / (bounce+1));
	*res += fsubres;
	return true;
}


// calculates direct light contribution for a ray
bool calc_ray_contrib(ray_t *ray, batch_blob_t *datablob, Pxf::Math::Vec3f *res, int bounce)
{
	bounce += 1;
	if (bounce >= datablob->bounce_count)
	{
		// Reached maximum reflection bounces
		return true;
	}
	
	triangle_t *closest_prim = 0x0;
	intersection_response_t closest_resp;
	if (find_intersection(datablob, ray, &closest_prim, &closest_resp))
	{
		/*
		*res = closest_resp.n;

		Vec3f n = closest_resp.n;

		n = (n + Vec3f(1.0f,1.0f,1.0f)) * 0.5;
		*res = n;

		return true; */

		//Pxf::Math::Vec3f eye_dir = ray.o - closest_resp.p;
		//Normalize(eye_dir);
		
		material_t m = datablob->materials.GetMaterial(closest_prim->material_index);
		*res += m.ambient;
	
		// loop lights
		for(int l = 0; l < datablob->light_count; ++l)
		{
			// Always assume we see the light
			float att = 1.0f;

			material_t l_mat = datablob->materials.GetMaterial(datablob->lights[l]->material_index);
		
			// create ray to light
			ray_t light_ray;
			light_ray.o = closest_resp.p;
			light_ray.d = ((BaseLight*) datablob->lights[l])->p - closest_resp.p;
			float light_distance = Length(light_ray.d);
			Normalize(light_ray.d);
			light_ray.o += light_ray.d*0.01f;
		
			// Point lights
			if (datablob->lights[l]->GetType() == PointLightPrim)
			{
				// See if we hit any geometry closer than the light
				intersection_response_t tresp;
				if (find_any_intersection_closer_than(datablob, &light_ray, light_distance, &tresp))
				{
					att = 0.0f;
				}
			
				// TODO: add better contributing calculations
				float ndotl = Dot(closest_resp.n, light_ray.d);
				if(ndotl > 0.0f)
					*res += m.diffuse * (l_mat.diffuse * ndotl * att) / (float)datablob->light_count;
					//*res += closest_prim->material->diffuse * (datablob->lights[l]->material->diffuse * ndotl * att) / (float)datablob->light_count;
		
			// Area lights
			} else if (datablob->lights[l]->GetType() == AreaLightPrim)
			{
				AreaLight *light = (AreaLight*)datablob->lights[l];
				
				// Calc distance
				float attscale = Pxf::Math::Clamp(light->strength / Length(((BaseLight*) light)->p - closest_resp.p), 0.0f, 1.0f);//1.0f / (float)datablob->light_count;
				
				// construct "up-vector"
				Vec3f up = Cross(light->normal, light->dir);
				float step_w = (float)light->width / (float)light->num_rays;
				float step_h = (float)light->height / (float)light->num_rays;
				Vec3f start_pos = light->p - (up * light->width + light->dir * light->height) / 2.0f;

				for(int y = 0; y < light->num_rays; ++y)
				{
					for(int x = 0; x < light->num_rays; ++x)
					{
						// get random sample
						//float rand_sample = datablob->samples[rand() % 255];
						float offset_x = 2.0f * (datablob->samples[rand() % 255] * 2.0f - 1.0f);//1.0f / ((int)(rand_sample * 16.0f) % 4);
						float offset_y = 2.0f * (datablob->samples[rand() % 255] * 2.0f - 1.0f);//0.0f;//1.0f / ((int)(rand_sample * 16.0f) / 4);
					
						// create light ray
						Vec3f light_point = start_pos + (up * step_w * ((float)x + offset_x)) + (light->dir * step_h * ((float)y + offset_y));
						light_ray.o = closest_resp.p;
						light_ray.d = light_point - closest_resp.p;
						light_distance = Length(light_ray.d);
						Normalize(light_ray.d);
						light_ray.o += light_ray.d*0.01f;
					
						intersection_response_t tresp;
						if (find_any_intersection_closer_than(datablob, &light_ray, light_distance, &tresp))
						{
							att -= (1.0f / ((float)light->num_rays * (float)light->num_rays));
						}
					}
				}
				
				// TODO: add better contributing calculations
				float ndotl = Dot(closest_resp.n, light_ray.d);
				float reflscale = 1.0f - m.reflectiveness;
				*res += m.diffuse * l_mat.diffuse * ndotl * attscale * reflscale * att;
				
				// Shoot reflection rays
				if (m.reflectiveness > 0.0f)
				{
					// Find reflection
					Pxf::Math::Vec3f bounce_contrib(0.0f);
					Pxf::Math::Vec3f eye_vec = closest_resp.p - ray->o;
					Normalize(eye_vec);
					Pxf::Math::Vec3f refl = closest_resp.n * (eye_vec - 2.0f * Dot(eye_vec, closest_resp.n));
					
					// reflection vector
					ray_t refl_ray;
					refl_ray.o = closest_resp.p;
					refl_ray.d = refl;
					Normalize(refl_ray.d);
					
					if (!calc_multisample_ray(&refl_ray, datablob, &bounce_contrib, m.matteness, bounce))
					{
						Pxf::Message("calc_ray_contrib", "Bounce calculations failed!");
						return false;
					}
					

					if (bounce_contrib.r < 0.0f)
						printf("error with bounce contrib!!\n");
						
					*res += (bounce_contrib / (float)bounce) * m.reflectiveness * attscale;
				}
			}
		

		}
	}
	
	return true;
}


bool calculate_pixel(float x, float y, task_detail_t *task, batch_blob_t *datablob, pixel_data_t *pixel)
{	
	// Clear pixel
	Pxf::Math::Vec3f fpixel(0.0f, 0.0f, 0.0f);
	fpixel.r = 0.0f;
	fpixel.g = 0.0f;
	fpixel.b = 0.0f;
	
	SimpleCamera* cam = (SimpleCamera*) datablob->cam;

	// Center ray
	Vec3f screen_coords(-1.0f + x, 1.0f - y,0.0f);
	ray_t cray;
	cray.o = Vec3f(0.0f,0.0f,1.41421356f);
	cray.o += cam->GetPos();

	Quaternion orientation = (*cam->GetOrientation());
	Normalize(orientation);

	//screen_coords = orientation * screen_coords;
	//Normalize(screen_coords);
	screen_coords += cam->GetPos();

	/*
	if (!calc_multisample_ray(&cray, datablob, &fpixel, 0.001f, 0))
	{
		Pxf::Message("calculate_pixel", "Ray shooting failed!");
		return false;
	}*/

	// find closest primitive
	for(int pixel_x = 0; pixel_x < datablob->samples_per_pixel; ++pixel_x)
	{
		for(int pixel_y = 0; pixel_y < datablob->samples_per_pixel; ++pixel_y)
		{
			// Offset ray
			ray_t ray = cray;
			Vec3f new_screen_coords = screen_coords;
			
			// TODO: Make sampling more non-uniform
			//new_screen_coords.x += (1.0f / ((float)datablob->pic_w * (float)datablob->samples_per_pixel)) * (float)pixel_x;
			//new_screen_coords.y += (1.0f / ((float)datablob->pic_h * (float)datablob->samples_per_pixel)) * (float)pixel_y;
			
			new_screen_coords.x += (0.5f / ((float)datablob->pic_w)) * (datablob->samples[rand() % 255] * 2.0f - 1.0f);
			new_screen_coords.y += (0.5f / ((float)datablob->pic_h)) * (datablob->samples[rand() % 255] * 2.0f - 1.0f);
			ray.d = new_screen_coords - cray.o;
			ray.d = orientation * ray.d;
			Normalize(ray.d);
			
			// Calc direct light
			Pxf::Math::Vec3f light_contrib;
			if (!calc_ray_contrib(&ray, datablob, &light_contrib, 0))
			{
				Pxf::Message("calculate_pixel", "Light calculations failed!");
				return false;
			}
			fpixel += light_contrib;
			
		}
	}
	
	fpixel /= datablob->samples_per_pixel * datablob->samples_per_pixel;
	
	fpixel.x = Pxf::Math::Clamp(fpixel.x, 0.0f, 1.0f);
	fpixel.y = Pxf::Math::Clamp(fpixel.y, 0.0f, 1.0f);
	fpixel.z = Pxf::Math::Clamp(fpixel.z, 0.0f, 1.0f);
		
	pixel->r = (unsigned char)(fpixel.r * 255.0f);
	pixel->g = (unsigned char)(fpixel.g * 255.0f);
	pixel->b = (unsigned char)(fpixel.b * 255.0f);	
	
	return true;
}
