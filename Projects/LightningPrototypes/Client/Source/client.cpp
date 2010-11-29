#include <Pxf/Base/String.h>
#include <Pxf/Base/Memory.h>
#include <zthread/ThreadedExecutor.h>

#include "client.h"
#include "iomodule.h"
#include "lightning.h"
#include "trackerclient.pb.h"

#include <sys/socket.h>

#include <stdio.h>

#define INITIAL_QUEUE 6

Client::Client(const char *_tracker_address, int _tracker_port, const char *_local_address, int _local_port)
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	m_tracker_address = Pxf::StringDuplicate(_tracker_address);
	m_tracker_port = _tracker_port;
	m_local_address = Pxf::StringDuplicate(_local_address);
	m_local_port = _local_port;

	m_TaskQueue.reserve(INITIAL_QUEUE); 

	m_ConnMan = ConnectionManager();
	m_Kernel = Pxf::Kernel::GetInstance();
	m_net_tag = m_ConnMan.m_log_tag;
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
	//ZThread::ThreadedExecutor batch_executor;
	//for (int i = 0; i < 5; i++)
	//{
	//		batch_executor.execute(new IOModule());
	//}
		
}

/* Connects to the tracker at the specified endpoint */

bool Client::connect_tracker()
{
	Connection *bound_c = m_ConnMan.new_connection(CLIENT);
	m_ConnMan.bind_connection(bound_c, m_local_address, m_local_port);
	bound_c->bound = true;

	Connection *c = m_ConnMan.new_connection(TRACKER);
	m_ConnMan.connect_connection(c, m_tracker_address, m_tracker_port);

	int type = INIT_HELLO;
	m_ConnMan.send(c, (char*)&type, 4);
	
	// Wait for tracker to respond. Timeout after 5 seconds.
	Pxf::Util::Array<LiPacket*> *packets = (Pxf::Util::Array<LiPacket*>*)m_ConnMan.recv_packets(5000);
	if (packets->size() == 0)
	{
		m_Kernel->Log(m_net_tag, "Connection to tracker at %s timed out.", c->target_address);
		return false;
	}
	
	trackerclient::HelloToClient *hello_client = (trackerclient::HelloToClient*)(packets->front()->unpack());
	
	m_session_id = hello_client->session_id();

	// TODO: Delete proper instance
	//delete msg;
	
	printf("Connected to tracker. Got session_id %d\n", m_session_id);

	trackerclient::HelloToTracker *hello_tracker = new trackerclient::HelloToTracker();
	hello_tracker->set_session_id(m_session_id);
	hello_tracker->set_address(m_local_address);
	hello_tracker->set_port(m_local_port);
	hello_tracker->set_available(m_TaskQueue.capacity()-m_TaskQueue.size());

	LiPacket *pkg = new LiPacket(c, hello_tracker, HELLO_TO_TRACKER);

	m_ConnMan.send(c, pkg->data, pkg->length);
	m_ConnMan.remove_connection(c);

	//m_ConnMan.recv()

	return true;
}

