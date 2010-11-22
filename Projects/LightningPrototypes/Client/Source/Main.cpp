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

	// Send ---------
	zmq_msg_t hello;
	int type = INIT_HELLO;
	zmq_msg_init_data(&hello, &type, sizeof(type), NULL, NULL);
	
	printf("Sending INIT_HELLO...\n");
	zmq_send(out_socket, &hello, 0);
	// --------------


	message* msg = recv_message(out_socket);

	PXF_ASSERT(msg->type == HELLO_TO_CLIENT, "Message type incorrect, expected HELLO_TO_CLIENT");

	String s_session_id = ((trackerclient::HelloToClient*)(msg->protobuf_data))->session_id();
	const char* sess = s_session_id.c_str();
	session_id = StringToInteger(sess);

	printf("session_id: %i\n", session_id);

	//delete(msg);

	zmq_msg_close(&hello);

	int aoeu = send_message(out_socket, msg);

	delete(msg);

	msg = recv_message(out_socket);

	printf("type:%i", msg->type);

	delete(msg);

	zmq_close(out_socket);
	zmq_close(in_socket);

	zmq_term(context);

	return(0);
}

