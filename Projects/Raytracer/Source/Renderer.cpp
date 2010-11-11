#include <Pxf/Math/Vector.h>

#include "Renderer.h"
#include "Intersections.h"

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
	pic->data = new pixel_data_t[region_width*region_height]; // width * height
	
	// Loop through all region pixels
	for(int y = 0; y < region_height; ++y)
	{
		for(int x = 0; x < region_width; ++x)
		{
			// Calculate pixel value
			if (!calculate_pixel(x, y, task, datablob, &pic->data[y*region_width+x]))
			{
				Pxf::Message("TaskRenderer", "Error while calculating pixel: [x: %d, y: %d]", x, y);
				return false;
			}
		}
	}
	
	// Return result
	return true;
}

bool calculate_pixel(int x, int y, task_detail_t *task, batch_blob_t *datablob, pixel_data_t *pixel)
{
	// TODO: Do some real calculations here!
	int region_width = task->region[2] - task->region[0];
	int region_height = task->region[3] - task->region[1];
	
	int rel_x = task->region[0] + x;
	int rel_y = task->region[1] + y;
	Pxf::Math::Vec3f world_coords(-(float)datablob->pic_w / 2.0f + (float)rel_x, (float)datablob->pic_h / 2.0f - (float)rel_y, 1.0f);
	
	
	pixel->r = (char)(255 * ((float)x / (float)region_width));
	pixel->g = (char)(255 * ((float)y / (float)region_height));
	pixel->b = (char)(255 * ((float)task->task_id / (float)task->task_count));
	
	// test sphere
	Pxf::Math::Vec3f sphere_c(0.0f,0.0f,5.0f);
	ray_t ray;
	ray.o = world_coords;
	Normalize(world_coords);
	ray.d = world_coords;
	intersection_response_t resp;
	
	if (ray_sphere(&sphere_c, 60, &ray, &resp))
	{
		resp.n = (resp.n + 1.0f) / 2.0f;
		pixel->r = resp.n.x * 255;
		pixel->g = resp.n.y * 255;
		pixel->b = resp.n.z * 255;
	}
	
	return true;
}
