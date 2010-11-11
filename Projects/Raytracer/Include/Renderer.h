#ifndef _RENDERER_H_
#define _RENDERER_H_

struct batch_blob_t
{
	char lols[256]; // Something stupid.
	// TODO: Add textures, kd-tree etc etc.
};

struct task_detail_t
{
	int region[4]; // x0,y0,x1,y1
	batch_blob_t *datablob;
	// TODO: Add camera stuff etc!
};

struct render_result_t
{
	char *data;
};

bool render_task(task_detail_t *task, render_result_t *pic);

#endif /* _RENDERER_H_ */