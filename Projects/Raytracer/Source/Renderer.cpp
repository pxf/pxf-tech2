#include "Renderer.h"

/**
 * Renders a task, using region etc and storing the data in pic.
 * Returns true if the task was successfully rendered, false otherwise.
 */
bool render_task(task_detail_t *task, batch_blob_t *datablob, render_result_t *pic)
{
	// TODO: Check incomming data!
	// (Don't know what to check atm... :/)
	
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
	
	pixel->r = (char)(255 * ((float)x / (float)region_width));
	pixel->g = (char)(255 * ((float)y / (float)region_height));
	pixel->b = (char)(255 * ((float)task->task_id / (float)task->task_count));
	return true;
}
