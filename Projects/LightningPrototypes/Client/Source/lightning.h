#include "trackerclient.pb.h"
#include "connectionmanager.h"

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
	google::protobuf::Message* protobuf_data;
};

message *unpack(Packet *pkg);
//message *recv_message(void* socket);
//int send_message(void* socket, message* msg);

void *get_proto_class(int type);

#endif

