#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Memory.h>

#include <zmq.hpp>

#include "lightning.h"

#include <stdio.h>


message::message()
	: type(0)
	, protobuf_data(NULL)
{ }
	
message::~message()
{
	printf("Deconstruct\n");
	if (protobuf_data != NULL)
		Pxf::MemoryFree(protobuf_data);
}

message *get_message(void* socket)
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

	protobuf_data[protobuf_size] = '\0';

	message* msg = new message;
	msg->type = message_type;
	msg->protobuf_data = protobuf_data;

	zmq_msg_close(&z_message_data);

	return msg;
}
