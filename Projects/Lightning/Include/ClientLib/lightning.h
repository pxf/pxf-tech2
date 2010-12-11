#ifndef _LIGHTNING_H_
#define _LIGHTNING_H_

#include <time.h>

#include <Pxf/Util/String.h>

#include "connectionmanager.h"
#include "lightning.pb.h"
#include "client.pb.h"
#include "tracker.pb.h"

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
	C_TASKS,
	C_RESULT
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

struct Task
{
	Batch* batch;
	client::Tasks::Task *task;
};

struct TaskResult
{
	Task* task;
	client::Result *result;
};

struct LiPacket : Packet
{
	LiPacket(Connection *_c, google::protobuf::Message *_proto, int _type);

	MessageType message_type;

	MessageType get_type();
	char *pack(google::protobuf::Message *_proto, int _type);
	google::protobuf::Message *unpack();
};

void *get_proto_class(MessageType type);

#endif

