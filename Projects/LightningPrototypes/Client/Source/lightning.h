#include "trackerclient.pb.h"

enum Message {
	INIT_HELLO, HELLO_TO_CLIENT, HELLO_TO_TRACKER, GOODBYE, NEWBATCH,
	OK, NODES_REQUEST, NODES_RESPONSE, BATCH_DONE, TASK_DONE, HATE, 
	PING, PONG
};

struct message {
	message();
	~message();

	int type;
	void* protobuf_data;
};

message *get_message(void* socket);

