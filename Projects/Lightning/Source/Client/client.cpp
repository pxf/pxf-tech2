#include "client.h"

#include <zthread/ConcurrentExecutor.h>

#define INITIAL_QUEUE 6
#define PING_INTERVAL 10000 // Ping interval in milliseconds
#define PING_TIMEOUT 5000 // Ping timeout in milliseconds

class SendThread : public ZThread::Runnable
{
protected:
	Client* m_Client;
	bool m_Canceled;
	ConnectionManager* m_ConnectionManager;
	Client::TaskResultQueue* m_ResultQueue;
	Pxf::Util::Map<Pxf::Util::String, Batch*>* m_BatchMap;
public:
	SendThread(Client* _client, Client::TaskResultQueue* _ResultQueue
			  ,Pxf::Util::Map<Pxf::Util::String, Batch*>* _BatchMap)
		: m_Client(_client)
		, m_Canceled(false)
		, m_ResultQueue(_ResultQueue)
		, m_BatchMap(_BatchMap)

	{
		m_ConnectionManager = new ConnectionManager();
	}

	void run()
	{
		while(!m_Canceled)
		{
			try
			{
				client::Result* result = m_ResultQueue->next();
				Pxf::Util::String batchhash = result->batchhash();
				Pxf::Util::Map<Pxf::Util::String, Batch*>::iterator \
					iter = m_BatchMap->find(batchhash);

				if (iter == m_BatchMap->end())
					continue; /* error */

				char* retaddr = (*iter).second->return_address;
				int retport = (*iter).second->return_port;

				Connection* conn = m_ConnectionManager->new_connection(CLIENT);
				m_ConnectionManager->connect_connection(conn, retaddr, retport);

				const char* data = result->result().c_str();
				int len = result->result().size();
				m_ConnectionManager->send(conn, (char*)data, len);
			}
			catch (ZThread::Cancellation_Exception* e)
			{
				m_Canceled = true;
				break;
			}
		}
	}
};

Client::Client(const char *_tracker_address, int _tracker_port, const char *_local_address, int _local_port, int _client_port)
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	m_tracker_address = Pxf::StringDuplicate(_tracker_address);
	m_tracker_port = _tracker_port;
	m_local_address = Pxf::StringDuplicate(_local_address);
	m_local_port = _local_port;
	m_client_port = _client_port;

	m_ConnMan = ConnectionManager((Pxf::Util::Array<Packet*>*)(new Pxf::Util::Array<LiPacket*>));
	m_State = State();

	m_Kernel = Pxf::Kernel::GetInstance();
	m_TaskQueue = new BlockingTaskQueue<Task*>;
	m_TaskQueue->register_type(RAYTRACER); // TODO: Move to raytracer class
	m_ResultQueue = new TaskResultQueue();

	m_ThreadedExecutor = new ZThread::ThreadedExecutor();
	m_ThreadedExecutor->execute(new SendThread(this, m_ResultQueue, &m_Batches));

	m_log_tag = m_Kernel->CreateTag("cli");
	m_net_tag = m_ConnMan.m_log_tag;
}

Client::~Client()
{
	m_ThreadedExecutor->cancel();
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
		// Check for old batches.
		if ((time(NULL) - last_batch_check) >= 60)
		{
			Pxf::Util::Map<Pxf::Util::String, Batch*>::iterator iter;
			for(iter = m_Batches.begin(); iter != m_Batches.end();)
			{
				if ((time(NULL) - ((*iter).second)->timestamp) > 60*10) // Allow 10 minutes idle.
				{
					// It's old.
					delete (*iter).second;
					m_Batches.erase(iter);
				}
				else
					iter++;
			}
			last_batch_check = time(NULL);
		}

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
			switch((*p)->message_type)
			{
				case PING:
				{
					lightning::Ping *ping = (lightning::Ping*)((*p)->unpack());
					lightning::Pong *pong = new lightning::Pong();
					pong->set_ping_data(ping->ping_data());

					LiPacket *pkg = new LiPacket((*p)->connection, pong, PONG);

					m_ConnMan.send((*p)->connection, pkg->data, pkg->length);

					delete pkg;
					delete pong;

					break;
				}
				case PONG: //PONG
				{
					lightning::Pong *pong = (lightning::Pong*)((*p)->unpack());
					(*p)->connection->timestamp = time(NULL);

					break;
				}
				case OK:
				{
					client_state* state = m_State.m_States[(*p)->connection];
					/*if (state->state == WOK)
					{
						switch (state->waitfor)
						{
							case WaitFor::HELLO:
							{
								//s
								break;
							}
						}
					}*/
					// TODO: Check what the connection is waiting for with the State class
					break;	
				}
				case GOODBYE:
				{
					m_Kernel->Log(m_log_tag, "Client %d disconnected.", (*p)->connection->session_id);
					m_ConnMan.remove_connection((*p)->connection);
					break;	
				}
				case C_HELLO:
				{
					// TODO: Do more stuff
					client::Hello *hello = (client::Hello*)((*p)->unpack());
					(*p)->connection->session_id = hello->session_id();
					(*p)->connection->type = CLIENT;

					m_Kernel->Log(m_log_tag, "Hello from %s:%d, session id:%d",
								  hello->address().c_str(),
								  hello->port(),
								  hello->session_id());

					int type = OK;
					m_ConnMan.send((*p)->connection, (char*)&type, 4);

					break;
				}
				case C_ALLOCATE:
				{
					// TODO: Depending on what is said to the client, put client in allocated list, or something
					if ((*p)->connection->type != CLIENT)
					{
						m_Kernel->Log(m_log_tag, "Client trying to allocate, but hasn't been introduced! Dropping connection.");

						m_ConnMan.remove_connection((*p)->connection);
						break;
					}

					client::AllocateClient *alloc = (client::AllocateClient*)((*p)->unpack());
					Pxf::Util::String hash = alloc->batchhash();

					client::AllocateResponse *alloc_resp = new client::AllocateResponse();
					alloc_resp->set_isavailable(m_queue_free > 0);
					alloc_resp->set_hasdata(m_Batches.count(hash) == 1);
		
					LiPacket *pkg = new LiPacket((*p)->connection, alloc_resp, C_ALLOC_RESP);

					m_Kernel->Log(m_log_tag, "Allocation request from %d. Granted.", (*p)->connection->session_id);

					// Tell the state
					m_State.m_Allocatees.push_back((*p)->connection);

					m_ConnMan.send((Packet*)pkg);

					delete alloc_resp;
					delete pkg;
				
					break;
				}
				case C_DATA:
				{
					client::Data *data = (client::Data*)((*p)->unpack());

					Pxf::Util::String hash = data->batchhash();

					// Check that client has allocated the resource
					// TODO: And that stuff doesn't exist yet
					if (count(m_State.m_Allocatees.begin(), m_State.m_Allocatees.end(), (*p)->connection) != 1)
					{
						m_Kernel->Log(m_log_tag, "Client has not been allocated!");
						delete data;
						break;
					}

					Batch *b = new Batch();
					
					b->hash = (char*)Pxf::MemoryAllocate(hash.length() + 1);
					hash.copy(b->hash, hash.length());
					b->hash[hash.length()] = '\0';

					b->type = (BatchType)data->datatype();

					b->data_size = data->datasize();
					b->data = (char*)Pxf::MemoryAllocate(b->data_size);
					Pxf::MemoryCopy(
						b->data,
						data->data().c_str(),
						b->data_size
					);

					b->timestamp = time(NULL);
					
					Pxf::Util::String str_address = data->returnaddress();
					b->return_address = (char*)Pxf::MemoryAllocate(str_address.length() + 1);
					str_address.copy(b->return_address, str_address.length());
					b->return_address[str_address.length()] = '\0';

					b->return_port = data->returnport();

					// Add batch to hashmap
					m_Batches[hash] = b;

					m_Kernel->Log(m_log_tag, "DATA from %d of type %d.", (*p)->connection->session_id, b->type);

					delete data;

					break;
				}
				case C_TASKS:
				{
					client::Tasks *proto_tasks = (client::Tasks*)((*p)->unpack());

					if (m_Batches.count(proto_tasks->batchhash()) != 1)
					{
						m_Kernel->Log(m_log_tag, "Tasks sent without knowing batch data, dropping tasks!");
						// TODO: Send tasks to another client?
						delete proto_tasks;
						break;
					}

					// Split the tasks in subgroups, copies data.
					Pxf::Util::Array<client::Tasks*> tasks = split_tasks(proto_tasks);

					delete proto_tasks;

					m_Kernel->Log(m_log_tag, "Pushing %d tasks from %d",
						tasks.back()->task_size(),
						(*p)->connection->session_id
					);

					// Push a set of tasks to our queue
					push(tasks.back());
					
					// Remove the set we just used
					tasks.pop_back();

					// Forward the rest
					forward(tasks);

					// MASSIVE CODE GOES HERE

					break;
				}
				case T_NODES_RESPONSE:
				{
					tracker::NodesResponse *nodes = (tracker::NodesResponse*)((*p)->unpack());
					m_Kernel->Log(m_log_tag, "Allocation response from tracker, got %d nodes", nodes->nodes_size());

					delete nodes;
					break;
				}
				default:
					m_Kernel->Log(m_log_tag, "Unknown packet type: %d", (*p)->message_type);
					break;
			}

			delete (*p);
			p = packets->erase(p);
		}

		//list_connections();

	}

	return 0;
}

// Initiate forwarding of tasks
void Client::forward(Pxf::Util::Array<client::Tasks*> _tasks)
{
	int diff = _tasks.size() - m_State.m_Allocated.size();

	if (diff > 0)
	{
		// More nodes needed
		tracker::NodesRequest* request = new tracker::NodesRequest();
		request->set_session_id(m_session_id);
		request->set_nodes(diff);

		LiPacket* pkg = new LiPacket(m_tracker, request, T_NODES_REQUEST);
		m_ConnMan.send((Packet*)pkg);

		m_Kernel->Log(m_log_tag, "Requested %d nodes of tracker.", diff);

		delete pkg;
		delete request;
	}
	
	//if (_tasks.size() - diff > 0)
}


// Pushes a set of tasks to the local queue
void Client::push(client::Tasks* _tasks)
{
	Batch* b = m_Batches[_tasks->batchhash()];

	for (int i = 0; i < _tasks->task_size(); i++)
	{
		Task* t = new Task();
		t->batch = b;
		client::Tasks::Task* n_task = new client::Tasks::Task();
		n_task->CopyFrom(_tasks->task(i));
		t->task = n_task;

		m_TaskQueue->push(b->type, t);
	}
}

// Split the tasks in chunks
Pxf::Util::Array<client::Tasks*> Client::split_tasks(client::Tasks* _tasks)
{
	int parts = 2; // TODO: Smarter stuff!
	int n = _tasks->task_size() / parts;
	Pxf::Util::Array<client::Tasks*> tasks;

	// Create new tasks sets
	for (int i = 0; i < parts; i++)
	{
		client::Tasks* t = new client::Tasks();
		t->set_batchhash(_tasks->batchhash());
		tasks.push_back(t);
	}

	int b = 0;
	for (int i = 0; i < _tasks->task_size(); i++) {
		client::Tasks_Task* t = tasks[b%parts]->add_task();
		t->CopyFrom(_tasks->task(i));
		b++;
	}
	
	return tasks;
}


void Client::ping(Connection *_c, int _timestamp)
{
	lightning::Ping *ping_tracker = new lightning::Ping();
	ping_tracker->set_ping_data(_timestamp);

	LiPacket *pkg = new LiPacket(_c, ping_tracker, PING);

	m_ConnMan.send(_c, pkg->data, pkg->length);

	delete pkg;
}

void Client::list_connections()
{
	Pxf::Util::Array<Connection*> cs = m_ConnMan.m_Connections;
	Pxf::Util::Array<Connection*>::iterator i;

	printf("Current open connections:\n");
	printf("=========================\n");

	for (i = cs.begin(); i != cs.end(); i++)
		printf("socket: %d, session_id: %d, type: %d, bound: %s timestamp: %d\n", 
			(*i)->socket,
			(*i)->session_id,
			(*i)->type,
			(*i)->bound ? "true" : "false",
			(*i)->timestamp
		);
	
	printf("=========================\n");
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

	int type = T_INIT;
	m_ConnMan.send(c, (char*)&type, 4);
	
	// Wait for tracker to respond. Timeout after 5 seconds.
	Pxf::Util::Array<LiPacket*> *packets = (Pxf::Util::Array<LiPacket*>*)m_ConnMan.recv_packets(5000);
	if (packets->size() == 0)
	{
		m_Kernel->Log(m_net_tag, "Connection to tracker at %s timed out.", c->target_address);
		return false;
	}
	
	tracker::HelloToClient *hello_client = (tracker::HelloToClient*)(packets->front()->unpack());
	
	m_session_id = hello_client->session_id();

	delete packets->front();
	packets->clear();
	
	m_Kernel->Log(m_log_tag, "Connected to tracker. Got session_id %d, using socket %d", m_session_id, c->socket);

	tracker::HelloToTracker *hello_tracker = new tracker::HelloToTracker();
	hello_tracker->set_session_id(m_session_id);
	hello_tracker->set_address(m_local_address);
	hello_tracker->set_port(m_local_port);
	hello_tracker->set_client_port(m_client_port);
	// TODO: Fix:
	//hello_tracker->set_available(m_TaskQueue.capacity()-m_TaskQueue.size());
	hello_tracker->set_available(6);

	LiPacket *pkg = new LiPacket(c, hello_tracker, T_HELLO_TRACKER);

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
			if (((*c)->type == TRACKER) && !((*c)->bound))
			{
				m_tracker = *c;
				return true;
			}

		m_Kernel->Log(m_net_tag, "Tracker could not connect to client. Are the ports open?");
		return false;
	}
	else
	{
		m_Kernel->Log(m_net_tag, "Got unknown packet when expecting connection from tracker.");

		delete packets->front();
		return false;
	}
}

