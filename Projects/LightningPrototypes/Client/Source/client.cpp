#include "client.h"

#define INITIAL_QUEUE 6
#define PING_INTERVAL 10000 // Ping interval in milliseconds
#define PING_TIMEOUT 5000 // Ping timeout in milliseconds

Client::Client(const char *_tracker_address, int _tracker_port, const char *_local_address, int _local_port)
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	m_tracker_address = Pxf::StringDuplicate(_tracker_address);
	m_tracker_port = _tracker_port;
	m_local_address = Pxf::StringDuplicate(_local_address);
	m_local_port = _local_port;

	m_TaskQueue.reserve(INITIAL_QUEUE); 

	// TODO: Change to LiPacket
	//m_ConnMan = ConnectionManager(new Pxf::Util::Array<LiPacket*>);
	m_ConnMan = ConnectionManager(new Pxf::Util::Array<Packet*>);
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
	int logtag = m_Kernel->CreateTag("cli");
	time_t last_ping, ping_timestamp;

	// TODO: Change to LiPacket
	//Pxf::Util::Array(<LiPacket*>) *packets;
	Pxf::Util::Array<Packet*> *packets;
	
	m_Kernel->Log(logtag, "Connecting to tracker at %s.", m_tracker_address);
	
	bool exit = false;

	if (!connect_tracker())
	{
		m_Kernel->Log(logtag, "Connection failed, quitting");
		exit = true;
	}
	
	ping_timestamp = time(NULL);
	
	// This is our main fail
	while(!exit)
	{
		packets = m_ConnMan.recv_packets(PING_INTERVAL);

		if (difftime(ping_timestamp, time(NULL)) > PING_INTERVAL/1000)
		{
			// Ping all connections	
			Pxf::Util::Array<struct Connection*>::iterator i_conn;
			for (i_conn = m_ConnMan.m_Connections.begin(); i_conn != m_ConnMan.m_Connections.end(); i_conn++)
			{
				if (difftime((*i_conn)->timestamp, ping_timestamp) > PING_TIMEOUT)
				{
					m_ConnMan.remove_connection(*i_conn);
					continue;
				}

				ping(*i_conn, (int)time(NULL));
			}

			ping_timestamp = time(NULL);

			continue;
		}

		// If there are no packets available, try to get some more
		if (packets->empty()) continue;

		// Packet loop
		// TODO: Change to LiPacket
		Pxf::Util::Array<Packet*>::iterator p;
		p = packets->begin();
		while (p != packets->end())
		{
			switch((*p)->length)
			{
				case PONG:
					m_Kernel->Log(logtag, "Got PONG from %s", (*p)->connection->target_address);
					(*p)->connection->timestamp = time(NULL);
					p = packets->erase(p);
					continue;
				default:
					m_Kernel->Log(logtag, "Unknown packet type: %d", (*p)->length); // TODO: LiPacket...
			}
			
			p++;
		}
	}
}


void Client::ping(Connection *_c, int _timestamp)
{
	char *data = (char*)Pxf::MemoryAllocate(2*sizeof(int));

	int t = PING;
	Pxf::MemoryCopy(
		data,
		&t,
		sizeof(t)
	);

	Pxf::MemoryCopy(
		data+sizeof(t),
		&_timestamp,
		sizeof(_timestamp)
	);

	m_ConnMan.send(_c, data, 2*sizeof(int));
	_c->timestamp = _timestamp;

	Pxf::MemoryFree(data);
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
	Pxf::Util::Array<Packet*> *packets = m_ConnMan.recv_packets(5000);
	if (packets->size() == 0)
	{
		m_Kernel->Log(m_net_tag, "Connection to tracker at %s timed out.", c->target_address);
		return false;
	}

	message *msg = unpack(packets->front());
	trackerclient::HelloToClient *hello_client = (trackerclient::HelloToClient*)(msg->protobuf_data);
	
	m_session_id = hello_client->session_id();

	delete msg;

	printf("Connected to tracker. Got session_id %d\n", m_session_id);

	trackerclient::HelloToTracker *hello_tracker = new trackerclient::HelloToTracker();
	hello_tracker->set_session_id(m_session_id);
	hello_tracker->set_address(m_local_address);
	hello_tracker->set_port(m_local_port);
	hello_tracker->set_available(m_TaskQueue.capacity()-m_TaskQueue.size());

	msg = new message;
	msg->type = HELLO_TO_TRACKER;
	msg->protobuf_data = hello_tracker;

	char *data = pack(msg);

	m_ConnMan.send(c, data, sizeof(msg->type)+hello_tracker->ByteSize());
	m_ConnMan.remove_connection(c);

	//m_ConnMan.recv()

	return true;
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
