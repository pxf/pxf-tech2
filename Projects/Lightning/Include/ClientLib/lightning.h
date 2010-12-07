#include <time.h>

#include "connectionmanager.h"
#include "lightning.pb.h"
#include "client.pb.h"
#include "tracker.pb.h"

#ifndef _LIGHTNING_H_
#define _LIGHTNING_H_

enum MessageType {
	// Common between all instances
	OK,
	PING,
	PONG,
	GOODBYE, 

	// Between tracker and client
	T_INIT,
	T_HELLO_CLIENT, 
	T_HELLO_TRACKER, 
	T_NEWBATCH,	
	T_NODES_REQUEST,
	T_NODES_RESPONSE,
	T_BATCH_DONE,
	T_TASK_DONE,
	T_HATE,

	// Between clients
	C_HELLO,
	C_ALLOCATE,
	C_ALLOC_RESP,
	C_DATA,
	C_TASKS
};

enum BatchType {
	RAYTRACER
};

struct Batch
{
	char *hash;		// Null terminated
	BatchType type;
	char *data;
	int data_size;
	time_t timestamp;
	char *return_address; // Null terminated
	int return_port;
};

struct LiPacket : Packet
{
	LiPacket(Connection *_c, google::protobuf::Message *_proto, int _type);

	MessageType message_type;

	MessageType get_type();
	char *pack(google::protobuf::Message *_proto, int _type);
	google::protobuf::Message *unpack();
};

void *get_proto_class(int type);

#endif

