#include "lightning.h"

#include <Pxf/Base/Debug.h>
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
		case OK: return(NULL);
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

message *unpack(Packet *pkg)
{
	// First 4 bytes of a message determines the type
	int message_type = (int)*(pkg->data);

	message* msg = new message;
	msg->type = message_type;
	msg->protobuf_data = NULL;

	google::protobuf::Message* buffered_message = get_protobuf_class(message_type);

	// Comment this
	if (buffered_message == NULL) return msg;

	// TODO: Error checking
	buffered_message->ParseFromString(pkg->data+sizeof(message_type));

	msg->protobuf_data = buffered_message;

	return msg;
}

char *pack(message *msg)
{
	char *data = (char*)Pxf::MemoryAllocate(sizeof(msg->type) + msg->protobuf_data->ByteSize());

	Pxf::MemoryCopy(
		data,
		&(msg->type),
		sizeof(msg->type)
	);

	Pxf::MemoryCopy(
		data+sizeof(msg->type),
		msg->protobuf_data->SerializeAsString().c_str(),
		msg->protobuf_data->ByteSize()
	);

	return data;
}

/*
message *recv_message(void* socket)
{
	int message_type;

	zmq_msg_t z_message_data;
	zmq_msg_init(&z_message_data);

	PXF_ASSERT(zmq_recv(socket, &z_message_data, 0) == 0, "ERROR ON zmq_recv().");

	char* message_data = (char*)zmq_msg_data(&z_message_data);
	
	// First 4 bytes of a message determines the type
	Pxf::MemoryCopy(&message_type, message_data, sizeof(message_type));

	message* msg = new message;
	msg->type = message_type;
	msg->protobuf_data = NULL;

	google::protobuf::Message* buffered_message = get_protobuf_class(message_type);

	// Comment this
	if (buffered_message == NULL) return msg;

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

	

	PXF_ASSERT(buffered_message->ParseFromString(protobuf_data), "Unable to parse protocol buffer data.");

	Pxf::MemoryFree(protobuf_data);

	msg->protobuf_data = buffered_message;

	return msg;
}

int send_message(void* socket, message* msg)
{
	zmq_msg_t z_message;
	//zmq_msg_init_data(&z_message, type+protobuf_data, msg->~message);
	int z_data_size = sizeof(msg->type)+msg->protobuf_data->ByteSize();
	char* message_data = (char*)Pxf::MemoryAllocate(z_data_size);

	Pxf::MemoryCopy(
		message_data+sizeof(msg->type), 
		msg->protobuf_data->SerializeAsString().c_str(),
		msg->protobuf_data->ByteSize()
	);

	Pxf::MemoryCopy(
		message_data,
		&(msg->type),
		sizeof(msg->type)
	);

	zmq_msg_init_data(&z_message, message_data, z_data_size, NULL, NULL);

	int ret = zmq_send(socket, &z_message, 0);

	zmq_msg_close(&z_message);
	
	return ret;
}
*/

