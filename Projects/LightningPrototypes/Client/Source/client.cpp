#include <Pxf/Base/String.h>
#include <Pxf/Base/Memory.h>
#include <zthread/ThreadedExecutor.h>
#include <zmq.hpp>

#include "client.h"
#include "iomodule.h"
#include "lightning.h"
#include "trackerclient.pb.h"

#include <stdlib.h>
#include <stdio.h>

Client::Client(const char *tracker_address, const char *local_address)
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	this->tracker_address = Pxf::StringDuplicate(tracker_address);
	this->local_address = Pxf::StringDuplicate(local_address);

	// TODO: Error checking
	context = zmq_init(1);
	out_socket = zmq_socket(context, ZMQ_REQ);
	in_socket = zmq_socket(context, ZMQ_REP);

	available = 4;
}

Client::~Client()
{
	Pxf::MemoryFree(tracker_address);
	Pxf::MemoryFree(local_address);

	zmq_close(out_socket);
	zmq_close(in_socket);

	zmq_term(context);
}

int Client::run()
{
	printf("Connecting to tracker at %s\n", tracker_address);
	//if (client.connect_tracker(client.tracker_address) == -1 )
	//{
	//	printf("Could not connect to tracker at %s.\n", client.tracker_address);
	//	return(-1);
	//}
	//printf("session_id:%s\n",client.session_id);

	// Simulate event
	ZThread::ThreadedExecutor batch_executor;
	for (int i = 0; i < 5; i++)
	{
			batch_executor.execute(new IOModule());
		
	}
		
}

/* Connects to the tracker at the specified endpoint */
int Client::connect_tracker(const char *address)
{
	// If we are unable to connect, a NULL pointer is returned.
	// TODO: Report error properly
	if(zmq_connect(this->out_socket, address) != 0) return -1;

	int msg_type = INIT_HELLO;
	zmq_msg_t init_hello;
	zmq_msg_init_data(&init_hello, &msg_type, sizeof(msg_type), NULL, NULL);

	zmq_send(this->out_socket, &init_hello, 0);

	message* credentials = recv_message(out_socket);

	if(credentials->type != HELLO_TO_CLIENT)
	{
		// Message is not what is expected
		// TODO: Report error properly
		return -1;
	}

	trackerclient::HelloToClient* hello_client = (trackerclient::HelloToClient*)(credentials->protobuf_data);

	session_id = (char*)Pxf::MemoryAllocate(hello_client->session_id().length());
	Pxf::MemoryCopy(
		session_id,
		hello_client->session_id().c_str(),
		hello_client->session_id().length()
	);

	delete(credentials);

	zmq_setsockopt(in_socket, ZMQ_IDENTITY, session_id, Pxf::StringLength(session_id));
	zmq_bind(in_socket, local_address);

	trackerclient::HelloToTracker* hello_tracker = new trackerclient::HelloToTracker();
	hello_tracker->set_session_id(this->session_id);
	hello_tracker->set_address(local_address);
	hello_tracker->set_available(available);

	message* msg = new message;
	msg->type=HELLO_TO_TRACKER;
	msg->protobuf_data = hello_tracker;

	send_message(out_socket, msg);

	delete(msg); // Will delete protocol buffer data as well

	msg = recv_message(out_socket);

	if (msg->type != OK) {
		delete(msg);
		return -1;
	}

	delete(msg);

	return 0;
}

