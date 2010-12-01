#include "client.h"

#define INITIAL_QUEUE 6
#define PING_INTERVAL 1000 // Ping interval in milliseconds
#define PING_TIMEOUT 500 // Ping timeout in milliseconds

Client::Client(const char *_tracker_address, int _tracker_port, const char *_local_address, int _local_port)
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	m_tracker_address = Pxf::StringDuplicate(_tracker_address);
	m_tracker_port = _tracker_port;
	m_local_address = Pxf::StringDuplicate(_local_address);
	m_local_port = _local_port;
	printf("m_local_addres : %s   m_local_port : %d\n", m_local_address, m_local_port);

	m_TaskQueue.reserve(INITIAL_QUEUE); 

	// TODO: Change to LiPacket
	m_ConnMan = ConnectionManager((Pxf::Util::Array<Packet*>*)(new Pxf::Util::Array<LiPacket*>));
	//m_ConnMan = ConnectionManager(new Pxf::Util::Array<Packet*>);
	m_Kernel = Pxf::Kernel::GetInstance();
	m_log_tag = m_Kernel->CreateTag("cli");
	m_net_tag = m_ConnMan.m_log_tag;
}

Client::~Client()
{
	Pxf::MemoryFree(m_tracker_address);
	Pxf::MemoryFree(m_local_address);
}

int Client::run()
{
	time_t last_ping, ping_timestamp;

	// TODO: Change to LiPacket
	Pxf::Util::Array<LiPacket*> *packets;
	//Pxf::Util::Array<Packet*> *packets;
	
	m_Kernel->Log(m_log_tag, "Connecting to tracker at %s.", m_tracker_address);
	
	bool exit = false;

	if (!connect_tracker())
	{
		m_Kernel->Log(m_log_tag, "Connection failed, quitting");
		exit = true;
	}
	
	ping_timestamp = time(NULL);
	
	// This is our main fail
	while(!exit)
	{
		packets = (Pxf::Util::Array<LiPacket*>*)m_ConnMan.recv_packets(PING_INTERVAL);

		if (difftime(time(NULL), ping_timestamp) > PING_INTERVAL/1000)
		{
			// Ping all connections	
			Pxf::Util::Array<struct Connection*>::iterator i_conn;
			for (i_conn = m_ConnMan.m_Connections.begin(); i_conn != m_ConnMan.m_Connections.end(); i_conn++)
			{
				if ((*i_conn)->bound)
					continue;

				if (difftime((*i_conn)->timestamp, ping_timestamp) > PING_TIMEOUT)
				{
					m_Kernel->Log(m_log_tag, "Connection to %s timed out...", (*i_conn)->target_address);
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
		Pxf::Util::Array<LiPacket*>::iterator p;
		p = packets->begin();
		while (p != packets->end())
		{
			switch((*p)->message_type)
			{
				case PONG:
					m_Kernel->Log(m_log_tag, "Got PONG from %s", (*p)->connection->target_address);
					(*p)->connection->timestamp = time(NULL);
					p = packets->erase(p);
					continue;
				default:
					m_Kernel->Log(m_log_tag, "Unknown packet type: %d", (*p)->message_type); // TODO: LiPacket...
			}
			
			p++;
		}
	}
}


void Client::ping(Connection *_c, int _timestamp)
{
	trackerclient::Ping *ping_tracker = new trackerclient::Ping();
	ping_tracker->set_ping_data(_timestamp);

	LiPacket *pkg = new LiPacket(_c, ping_tracker, PING);

	m_ConnMan.send(_c, pkg->data, pkg->length);

	delete pkg;
	_c->timestamp = _timestamp;
}

/* Connects to the tracker at the specified endpoint */
bool Client::connect_tracker()
{
	Connection *bound_c = m_ConnMan.new_connection(CLIENT);
	if(!m_ConnMan.bind_connection(bound_c, m_local_address, m_local_port))
	{
		m_Kernel->Log(m_net_tag, "Could not bind to %s:%d", m_local_address, m_local_port);
		return false;
	}	
	bound_c->type = TRACKER;

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

	delete packets->front();
	packets->clear();
	
	m_Kernel->Log(m_log_tag, "Connected to tracker. Got session_id %d", m_session_id);

	trackerclient::HelloToTracker *hello_tracker = new trackerclient::HelloToTracker();
	hello_tracker->set_session_id(m_session_id);
	hello_tracker->set_address(m_local_address);
	hello_tracker->set_port(m_local_port);
	hello_tracker->set_available(m_TaskQueue.capacity()-m_TaskQueue.size());

	LiPacket *pkg = new LiPacket(c, hello_tracker, HELLO_TO_TRACKER);

	m_ConnMan.send(c, pkg->data, pkg->length);
	m_ConnMan.remove_connection(c);
	delete pkg;

	packets = (Pxf::Util::Array<LiPacket*>*)m_ConnMan.recv_packets(5000);

	if (packets->empty())
	{
		// Check that the tracker connected...
		Pxf::Util::Array<Connection*>::iterator c;
		for (c = m_ConnMan.m_Connections.begin(); c != m_ConnMan.m_Connections.end(); c++)
			if ((*c)->type == TRACKER) return true;

		m_Kernel->Log(m_net_tag, "Tracker could not connect to client. Are the ports open?");
		return false;
	}
	else
	{
		m_Kernel->Log(m_net_tag, "Got unknown packet when expecting connection from tracker.");
		return false;
	}
}

