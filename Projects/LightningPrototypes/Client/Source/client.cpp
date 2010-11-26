#include <Pxf/Base/String.h>
#include <Pxf/Base/Memory.h>
#include <zthread/ThreadedExecutor.h>

#include "client.h"
#include "iomodule.h"
#include "trackerclient.pb.h"

#include <sys/socket.h>

#include <stdio.h>

#define INITIAL_QUEUE 6

Client::Client(const char *_tracker_address, int _tracker_port, const char *_local_address)
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	m_tracker_address = Pxf::StringDuplicate(_tracker_address);
	m_tracker_port = _tracker_port;
	m_local_address = Pxf::StringDuplicate(_local_address);

	m_TaskQueue.reserve(INITIAL_QUEUE); 

	m_ConnMan = ConnectionManager();
}

Client::~Client()
{
	Pxf::MemoryFree(m_tracker_address);
	Pxf::MemoryFree(m_local_address);
}

int Client::run()
{
	printf("Connecting to tracker at %s\n", m_tracker_address);
	connect_tracker();
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

int Client::connect_tracker()
{
	Connection* c = m_ConnMan.new_connection(TRACKER);
	m_ConnMan.connect_connection(c, m_tracker_address, m_tracker_port);

//	char medd[] = "lol";
//	printf("send %d bytes.\n", send(3, medd, 4, 0));

	char buff[512];
	int l = 4;
	sprintf(buff, "0000hej\0");
	memcpy(buff, &l, 4);

	m_ConnMan.send(c, buff, 8);

	m_ConnMan.remove_connection(c);

	return(0);
}

	/*
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
*/
