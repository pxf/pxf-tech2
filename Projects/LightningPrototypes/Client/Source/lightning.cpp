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
		case PONG: return new trackerclient::Pong;
		case OK: return(NULL);
	}
}

LiPacket::LiPacket(Connection *_c, google::protobuf::Message *_proto, int _type)
{
	connection = _c;
	pack(_proto, _type);
}

MessageType LiPacket::get_type()
{
	message_type = (MessageType)*data;
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
		(int*)&message_type,
		data,
		sizeof(message_type)
	);

	google::protobuf::Message *proto = get_protobuf_class(message_type);

	if (proto == NULL) return NULL;

	proto->ParseFromString(data+sizeof(message_type));

	return proto;
}

