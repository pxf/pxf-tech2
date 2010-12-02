#include "trackerclient.pb.h"
#include "connectionmanager.h"

#ifndef _LIGHTNING_H_
#define _LIGHTNING_H_

enum MessageType {
	INIT_HELLO = 0,
	HELLO_TO_CLIENT = 1, 
	HELLO_TO_TRACKER, 
	GOODBYE, 
	NEWBATCH,	
	OK,
	NODES_REQUEST,
	NODES_RESPONSE,
	BATCH_DONE,
	TASK_DONE,
	HATE, 
	PING,
	PONG
};

struct LiPacket : Packet
{
	LiPacket(Connection *_c, google::protobuf::Message *_proto, int _type);

	MessageType message_type;
	char *pack(google::protobuf::Message *_proto, int _type);
	google::protobuf::Message *unpack();
};

void *get_proto_class(int type);

#endif

