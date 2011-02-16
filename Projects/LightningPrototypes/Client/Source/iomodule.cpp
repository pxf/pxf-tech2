#include <zthread/Thread.h>
#include <stdio.h>

#include "iomodule.h"

IOModule::IOModule()
{
	batch_type = HERPADERPA;
	return;
}

IOModule::~IOModule()
{
	return;
}

void IOModule::run()
{	
	printf("Running...\n");
	ZThread::Thread::sleep(1000);
	printf("Return...\n");
	return;
}
