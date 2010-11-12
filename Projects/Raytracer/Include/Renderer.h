#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <Pxf/Base/Debug.h>

struct batch_blob_t
{
	char lols[256]; // Something stupid for now..
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
bool calculate_pixel(float x, float y, task_detail_t *task, batch_blob_t *datablob, pixel_data_t *pixel);

#endif /* _RENDERER_H_ */