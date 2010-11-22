#include "trackerclient.pb.h"


#ifndef _CLIENT_H_
#define _CLIENT_H_

struct client {
	int session_id;
	char* address;
};

int connect_tracker(void* socket, const char* address);


#endif
