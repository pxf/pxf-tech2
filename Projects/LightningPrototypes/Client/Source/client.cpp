#include "client.h"

#define INITIAL_QUEUE 6
#define PING_INTERVAL 10000 // Ping interval in milliseconds
#define PING_TIMEOUT 5000 // Ping timeout in milliseconds

Client::Client(const char *_tracker_address, int _tracker_port, const char *_local_address, int _local_port, int _client_port)
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	m_tracker_address = Pxf::StringDuplicate(_tracker_address);
	m_tracker_port = _tracker_port;
	m_local_address = Pxf::StringDuplicate(_local_address);
	m_local_port = _local_port;
	m_client_port = _client_port;

	m_TaskQueue.reserve(INITIAL_QUEUE); 

	m_ConnMan = ConnectionManager((Pxf::Util::Array<Packet*>*)(new Pxf::Util::Array<LiPacket*>));
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

	Pxf::Util::Array<LiPacket*> *packets;
	
	// Setting up socket for clients to connect to
	Connection *client_c = m_ConnMan.new_connection(CLIENT);
	if(!m_ConnMan.bind_connection(client_c, m_local_address, m_client_port))
	{
		m_Kernel->Log(m_net_tag, "Could not bind to %s:%d", m_local_address, m_client_port);
		return false;
	}
	printf("bound client socket has sockid:%d\n", client_c->socket);

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

		if (difftime(time(NULL), ping_timestamp) > PING_INTERVAL/1000.0f)
		{
			// Ping all connections	
			Pxf::Util::Array<struct Connection*>::iterator i_conn;
			for (i_conn = m_ConnMan.m_Connections.begin(); i_conn != m_ConnMan.m_Connections.end(); i_conn++)
			{
				if ((*i_conn)->bound)
					continue;

				if (difftime(ping_timestamp, (*i_conn)->timestamp) > PING_TIMEOUT/1000.0f)
				{
					m_Kernel->Log(m_log_tag, "Connection to %s timed out...", (*i_conn)->target_address);
					m_ConnMan.remove_connection(*i_conn);
					if (i_conn == m_ConnMan.m_Connections.end())
						break;
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
			(*p)->get_type();
			if ((*p)->connection->type == CLIENT)
			{
				m_Kernel->Log(m_log_tag, "Packet from a client");
				
				switch((*p)->message_type)
				{
					case 1:
						break;
					default:
						m_Kernel->Log(m_log_tag, "Unknown packet type: %d", (*p)->message_type);
						p = packets->erase(p);
						continue;
				}
			}
			else if ((*p)->connection->type == TRACKER)
			{
				m_Kernel->Log(m_log_tag, "Packet from tracker");
				switch((*p)->message_type)
				{
					case PONG: //PONG
					{
						trackerclient::Pong *pong = (trackerclient::Pong*)((*p)->unpack());
						(*p)->connection->timestamp = time(NULL);
						p = packets->erase(p);
						continue;
					}
					default:
						m_Kernel->Log(m_log_tag, "Unknown packet type: %d", (*p)->message_type);
						p = packets->erase(p);
						continue;
				}
			}
			else if ((*p)->connection->type == INTERNAL)
			{
				1+1;
			}
			else
			{
				m_Kernel->Log(m_log_tag, "Packet from unknown connection type, dropping");
				p = packets->erase(p);
				continue;
			}
			
			p++;
		}
	}

	return 0;
}


void Client::ping(Connection *_c, int _timestamp)
{
	trackerclient::Ping *ping_tracker = new trackerclient::Ping();
	ping_tracker->set_ping_data(_timestamp);

	LiPacket *pkg = new LiPacket(_c, ping_tracker, PING);

	m_ConnMan.send(_c, pkg->data, pkg->length);

	delete pkg;
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
	if (!m_ConnMan.connect_connection(c, m_tracker_address, m_tracker_port))
	{
		m_Kernel->Log(m_net_tag, "Could not connect to tracker.");
		return false;
	}

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

//	delete packets->front();
	packets->clear();
	
	m_Kernel->Log(m_log_tag, "Connected to tracker. Got session_id %d, using socket %d", m_session_id, c->socket);

	trackerclient::HelloToTracker *hello_tracker = new trackerclient::HelloToTracker();
	hello_tracker->set_session_id(m_session_id);
	hello_tracker->set_address(m_local_address);
	hello_tracker->set_port(m_local_port);
	hello_tracker->set_client_port(m_client_port);
	hello_tracker->set_available(m_TaskQueue.capacity()-m_TaskQueue.size());

	LiPacket *pkg = new LiPacket(c, hello_tracker, HELLO_TO_TRACKER);

	m_ConnMan.send(c, pkg->data, pkg->length);
	m_Kernel->Log(m_log_tag, "Connection to tracker on socket %d terminated.", c->socket);
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

