#include <zthread/Runnable.h>

#ifndef _BATCHPROCESSOR_H_
#define _BATCHPROCESSOR_H_

enum BatchType {
	RAYTRACE,
	HERPADERPA
};

class BatchProcessor : public ZThread::Runnable
{
	public:
		int batch_type;
	private:
		void *get_tasks();
		void *task_complete(void *task_data, int task_id);
};

#endif
