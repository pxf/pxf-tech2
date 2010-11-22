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
#include "client.h"
#include "trackerclient.pb.h"
#include "clientclient.pb.h"

using namespace Pxf;
using namespace Util;

int main(int argc, char *argv[])
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	client* client_data;

	printf("Client.\n");
	printf("Connecting to tracker...\n");

	void *context = zmq_init(1);
	void *out_socket = zmq_socket(context, ZMQ_REQ);
	void *in_socket = zmq_socket(context, ZMQ_REP);

	int session_id = connect_tracker(out_socket, "tcp://users.mkeyd.net:50000");
	if(client_data == NULL)
	{
		printf("Unable to connect to tracker.\n");
		return(-1);
	}
	printf("session_id: %i\n", session_id);

	
	/*
	int aoeu = send_message(out_socket, msg);

	delete(msg);

	msg = recv_message(out_socket);

	printf("type:%i", msg->type);

	delete(msg);*/

	zmq_close(out_socket);
	zmq_close(in_socket);

	zmq_term(context);

	return(0);
}

