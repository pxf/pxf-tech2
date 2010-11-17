//#include <Pxf/SampleModule/SampleModule.h>
#include <stdio.h>

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

#include <Pxf/Base/Hash.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Memory.h>
#include <Pxf/Base/Random.h>

#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/ResourceLoader.h>

#include <zmq.hpp>

#include <ctime>

#include "tracker.pb.h"
#include "clientclient.pb.h"

using namespace Pxf;

enum Message {
	INIT_HELLO, HELLO_TO_CLIENT, HELLO_TO_TRACKER, GOODBYE, NEWBATCH,
	OK, NODES_REQUEST, NODES_RESPONSE, BATCH_DONE, TASK_DONE, HATE, 
	PING, PONG
};

int main()
{

	GOOGLE_PROTOBUF_VERIFY_VERSION;
	
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

	char* aoeu = (char*)zmq_msg_data(&reply);

	printf("Data recieved: %s\n", aoeu+4);

	zmq_msg_close(&reply);
	zmq_msg_close(&hello);



	zmq_close(out_socket);
	zmq_close(in_socket);

	zmq_term(context);

	return(0);
}
