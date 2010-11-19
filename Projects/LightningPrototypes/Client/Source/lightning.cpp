#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Memory.h>

#include <zmq.hpp>

#include "lightning.h"

#include <stdio.h>

/*
Factory[] = {
	Factory<trackerclient::HelloToClient(),
	Factory<trackerclient::HelloToTracker()
};*/

/*void *protoaoeu[3] {
	NULL,
	(void*)create_class<trackerclient::HelloToClient>,
	(void*)create_class<trackerclient::HelloToTracker>,
};*/

google::protobuf::Message *get_protobuf_class(int type)
{
	switch(type)
	{
		case INIT_HELLO: return(NULL);
		case HELLO_TO_CLIENT: return new trackerclient::HelloToClient;
		case HELLO_TO_TRACKER: return new trackerclient::HelloToTracker;
		case GOODBYE: return new trackerclient::GoodBye;
	}
}

message::message()
	: type(0)
	, protobuf_data(NULL)
{ }
	
message::~message()
{
	if (protobuf_data != NULL)
		delete protobuf_data;
}

message *recv_message(void* socket)
{
	int message_type;

	zmq_msg_t z_message_data;
	zmq_msg_init(&z_message_data);

	PXF_ASSERT(zmq_recv(socket, &z_message_data, 0) == 0, "ERROR ON zmq_recv().");

	char* message_data = (char*)zmq_msg_data(&z_message_data);
	
	// First 4 bytes of a message determines the type
	Pxf::MemoryCopy(&message_type, message_data, sizeof(message_type));

	int protobuf_size = zmq_msg_size(&z_message_data)-sizeof(message_type);
	char* protobuf_data = (char*)Pxf::MemoryAllocate(protobuf_size+1); // Extra byte for '\0'
	Pxf::MemoryCopy(
		protobuf_data,
		message_data+sizeof(message_type), // Start after the type int
		protobuf_size
	);

	zmq_msg_close(&z_message_data);

	// Make protobuf_data null terminating
	protobuf_data[protobuf_size] = '\0';

	google::protobuf::Message* buffered_message = get_protobuf_class(message_type);
	

	PXF_ASSERT(buffered_message->ParseFromString(protobuf_data), "Unable to parse protocol buffer data.");

	Pxf::MemoryFree(protobuf_data);


	message* msg = new message;
	msg->type = message_type;
	msg->protobuf_data = buffered_message;

	return msg;
}

