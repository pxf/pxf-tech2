#include "lightning.h"

google::protobuf::Message *get_protobuf_class(MessageType type)
{
	switch(type)
	{
		case GOODBYE: return new lightning::GoodBye;
		case PING: return new lightning::Ping;
		case PONG: return new lightning::Pong;
		case OK: return(NULL);

		case T_INIT: return(NULL);
		case T_HELLO_CLIENT: return new tracker::HelloToClient;
		case T_HELLO_TRACKER: return new tracker::HelloToTracker;
		case T_NEWBATCH: return new tracker::NewBatch;
		case T_NODES_REQUEST: return new tracker::NodesRequest;
		case T_NODES_RESPONSE: return new tracker::NodesResponse;
		case T_BATCH_DONE: return NULL; // TODO: Fill in
		case T_TASK_DONE: return NULL;
		case T_HATE: return NULL;

		case C_HELLO: return new client::Hello;
		case C_ALLOCATE: return new client::AllocateClient;
		case C_ALLOC_RESP: return new client::AllocateResponse;
		case C_DATA: return new client::Data;
		case C_TASKS: return new client::Tasks;
	}
}

LiPacket::LiPacket(Connection *_c, google::protobuf::Message *_proto, int _type)
{
	connection = _c;
	pack(_proto, _type);
}

MessageType LiPacket::get_type()
{
	message_type = (MessageType)*(int*)data;
	return message_type;
}


char *LiPacket::pack(google::protobuf::Message *_proto, int _type)
{
	length = sizeof(_type) + _proto->ByteSize();
	data = (char*)Pxf::MemoryAllocate(length);

	Pxf::MemoryCopy(
		data,
		&(_type),
		sizeof(_type)
	);

	Pxf::MemoryCopy(
		data+sizeof(_type),
		_proto->SerializeAsString().c_str(),
		_proto->ByteSize()
	);

	message_type = (MessageType)_type;

	return data;
}

google::protobuf::Message *LiPacket::unpack()
{
	Pxf::MemoryCopy(
		(MessageType*)&message_type,
		data,
		sizeof(message_type)
	);

	google::protobuf::Message *proto = get_protobuf_class(message_type);

	if (proto == NULL) return NULL;

	proto->ParseFromString(data+sizeof(message_type));

	return proto;
}

