#include "trackerclient.pb.h"

#ifndef _LIGHTNING_H_
#define _LIGHTNING_H_

enum Message {
	INIT_HELLO, HELLO_TO_CLIENT, HELLO_TO_TRACKER, GOODBYE, NEWBATCH,
	OK, NODES_REQUEST, NODES_RESPONSE, BATCH_DONE, TASK_DONE, HATE, 
	PING, PONG
};

template<typename T>
class Factory 
{
	public: T CreateInstance()
	{
		return new T;
	}
};

struct message {
	message();
	~message();

	int type;
	void* protobuf_data;
};

message *get_message(void* socket);

void *get_proto_class(int type);

#endif

