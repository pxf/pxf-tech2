#include <Pxf/Math/Vector.h>

#include "Renderer.h"
#include "Intersections.h"

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

bool calculate_pixel(float x, float y, task_detail_t *task, batch_blob_t *datablob, pixel_data_t *pixel)
{
	// TODO: Do some real calculations here!
	Vec3f screen_coords(-1.0f + x, 1.0f - y, 0.0f);
	
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
	Vec3f light(9.0f, 9.0f, 9.0f);
	
	if (ray_sphere(&sphere_c, 7, &ray, &resp))
	{
		resp.n = (resp.n + 1.0f) / 2.0f;
		pixel->r = resp.n.x * 255;
		pixel->g = resp.n.y * 255;
		pixel->b = resp.n.z * 255;
	}
	
	return true;
}
