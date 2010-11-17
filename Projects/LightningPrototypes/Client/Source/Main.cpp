//#include <Pxf/SampleModule/SampleModule.h>
#include <stdio.h>

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

#include <Pxf/Base/Hash.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Memory.h>
#include <Pxf/Base/Random.h>
#include <Pxf/Util/String.h>

#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/ResourceLoader.h>

#include <zmq.hpp>

#include <ctime>

#include "lightning.h"
#include "trackerclient.pb.h"
#include "clientclient.pb.h"

using namespace Pxf;
using namespace Util;

int main()
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	int session_id;

	printf("Client.\n");
	printf("Connecting to tracker...\n");

	void *context = zmq_init(1);
	void *out_socket = zmq_socket(context, ZMQ_REQ);
	void *in_socket = zmq_socket(context, ZMQ_REP);

	zmq_connect(out_socket, "tcp://users.mkeyd.net:50000");

	zmq_msg_t hello;
	int type = INIT_HELLO;
	zmq_msg_init_data(&hello, &type, sizeof(type), NULL, NULL);
	
	printf("Sending INIT_HELLO...\n");
	zmq_send(out_socket, &hello, 0);

	zmq_msg_t reply;
	zmq_msg_init(&reply);
	zmq_recv(out_socket, &reply, 0);

	char* reply_data = (char*)zmq_msg_data(&reply);
	int protobuf_size = zmq_msg_size(&reply)-sizeof(type)+1; // Extra byte for NULL char
	char* protobuf_data = (char*)malloc(protobuf_size);
	memcpy(&type, reply_data, sizeof(type));
	memcpy(
		protobuf_data,
		reply_data+sizeof(type),
		protobuf_size
	);
	
	protobuf_data[protobuf_size-1] = NULL;

	assert(type == HELLO_TO_CLIENT);

	trackerclient::HelloToClient hello_to_client;
	if (!hello_to_client.ParseFromString(protobuf_data)) 
	{
		printf("Unable to parse protobuf data!\n");
		return(-1);
	}

	free(protobuf_data);

	String s_session_id = hello_to_client.session_id();
	const char* sess = s_session_id.c_str();
	session_id = StringToInteger(sess);

	printf("session_id: %i\n", session_id);


	zmq_msg_close(&reply);
	zmq_msg_close(&hello);



	zmq_close(out_socket);
	zmq_close(in_socket);

	zmq_term(context);

	return(0);
}
