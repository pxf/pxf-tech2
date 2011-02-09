#include "batchprocessor.h"

#ifndef _IOMODULE_H_
#define _IOMODULE_H_

class IOModule : public BatchProcessor
{
	public:
		void run();
		IOModule();
		~IOModule();
};

#endif

