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
				client::Result* result = m_ResultQueue->next()->result;
				Pxf::Util::String batchhash = result->batchhash();
				Pxf::Util::Map<Pxf::Util::String, Batch*>::iterator \
					iter = m_BatchMap->find(batchhash);

				if (iter == m_BatchMap->end())
				{
					printf("Error, iterator is at m_BatchMap->end().\n");
					continue; /* error */
				}
					

				char* retaddr = (*iter).second->return_address;
				int retport = (*iter).second->return_port;

				Connection* conn = m_ConnectionManager->new_connection(CLIENT);
				m_ConnectionManager->connect_connection(conn, retaddr, retport);
				
				//std::string str = result->SerializeAsString();
				LiPacket* pkg = new LiPacket(conn, result, C_RESULT);
				m_ConnectionManager->send(pkg->connection, pkg->data, pkg->length);
				//m_ConnectionManager->send(conn, (char*)str.c_str(), str.size());
				m_ConnectionManager->remove_connection(conn);
			}
			catch (ZThread::Cancellation_Exception* e)
			{
					printf("ALLOCATION!");
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

	m_ConnMan = ConnectionManager();
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

	Pxf::Util::Array<Packet*> *packets;

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
	last_batch_check = time(NULL);
	
	// This is our main fail
	while(!exit)
	{
		// Check for old batches. 
		/* TODO: Make sure there are no current tasks still awaiting processing for the batch
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
		}*/

		packets = m_ConnMan.recv_packets(PING_INTERVAL);

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
		Pxf::Util::Array<Packet*>::iterator pack_iter;
		pack_iter = packets->begin();
		while (pack_iter != packets->end())
		{
			LiPacket* p = new LiPacket(*pack_iter);
			p->get_type();
			switch(p->message_type)
			{
				case PING:
				{
					lightning::Ping *ping = (lightning::Ping*)(p->unpack());
					lightning::Pong *pong = new lightning::Pong();
					pong->set_ping_data(ping->ping_data());

					LiPacket *pkg = new LiPacket(p->connection, pong, PONG);

					m_ConnMan.send(p->connection, pkg->data, pkg->length);

					delete pkg;
					delete pong;

					break;
				}
				case PONG: //PONG
				{
					lightning::Pong *pong = (lightning::Pong*)(p->unpack());
					p->connection->timestamp = time(NULL);

					break;
				}
				case OK:
				{
					client_state* state = m_State.m_States[p->connection];
					if (state->state == (WOK & W_HELLO))
					{
						// Waiting for OK from a HELLO request
						m_Kernel->Log(m_log_tag, "Got OK response on HELLO request from %d.",
							p->connection->session_id);

						// Adding client to known clients
						m_State.m_Clients.push_back(p->connection);

						// Send allocation request
						if (m_State.m_OutQueue.size() > 0)
						{
							Batch* b = m_Batches[m_State.m_OutQueue.front()->batchhash()];
							allocate_client(p->connection, b, state->send_tasks);
						}
						else
							m_Kernel->Log(m_log_tag, "No tasks to be forwarded, doing nothing.");
					}
					else
						m_Kernel->Log(m_log_tag, "Why did %d send an OK?", p->connection->session_id);

					// TODO: More stuff?
					break;	
				}
				case GOODBYE:
				{
					m_Kernel->Log(m_log_tag, "Client %d disconnected.", p->connection->session_id);
					m_ConnMan.remove_connection(p->connection);

					// TODO: Remove from m_State.m_Clients as well
					break;	
				}
				case C_HELLO:
				{
					// TODO: Do more stuff
					client::Hello *hello = (client::Hello*)(p->unpack());
					p->connection->session_id = hello->session_id();
					p->connection->type = CLIENT;

					m_Kernel->Log(m_log_tag, "Hello from %s:%d, session id:%d",
								  hello->address().c_str(),
								  hello->port(),
								  hello->session_id());

					int type = OK;
					m_ConnMan.send(p->connection, (char*)&type, 4);

					if (p->connection->session_id != -1)
						m_State.m_Clients.push_back(p->connection);

					break;
				}
				case C_ALLOCATE:
				{
					// TODO: Depending on what is said to the client, put client in allocated list, or something
					if (p->connection->type != CLIENT)
					{
						m_Kernel->Log(m_log_tag, "Client trying to allocate, but hasn't been introduced! Dropping connection.");

						m_ConnMan.remove_connection(p->connection);
						break;
					}

					client::AllocateClient *alloc = (client::AllocateClient*)(p->unpack());
					Pxf::Util::String hash = alloc->batchhash();

					// TODO: Check that the client actually is able to process the specified type
					// alloc->datatype();

					client::AllocateResponse *alloc_resp = new client::AllocateResponse();
					alloc_resp->set_isavailable(m_TaskQueue->get_available());
					alloc_resp->set_hasdata(m_Batches.count(hash) == 1);
					alloc_resp->set_batchhash(alloc->batchhash());
		
					LiPacket *pkg = new LiPacket(p->connection, alloc_resp, C_ALLOC_RESP);

					m_Kernel->Log(m_log_tag, "Allocation request from %d. %s."
						, p->connection->session_id
						, m_TaskQueue->get_available() ? "Granted" : "Denied" 
					);

					m_ConnMan.send(pkg->connection, pkg->data, pkg->length);

					delete alloc_resp;
					delete pkg;
				
					break;
				}
				case C_ALLOC_RESP:
				{
					m_Kernel->Log(m_log_tag, "Recieved allocation response.\n");
					client::AllocateResponse *resp = (client::AllocateResponse*)(p->unpack());
					
					if (!(resp->isavailable()))
					{
						m_Kernel->Log(m_log_tag, "Client has no free slots, not sending tasks.\n");


						// TODO: Do something smart, blacklist? Can't remember :(
						// But don't send any tasks
						// Don't add to allocated list.

						delete resp;
						break;
					}
						
					if (!(resp->hasdata()))
					{
						m_Kernel->Log(m_log_tag, "Target client does not have data, sending...");
						Batch* b = m_Batches[resp->batchhash()];
						client::Data* data = new client::Data();
						data->set_batchhash(b->hash);
						data->set_datasize(b->data_size);
						data->set_datatype(b->type);
						data->set_data(b->data);
						data->set_returnaddress(b->return_address);
						data->set_returnport(b->return_port);

						LiPacket* pkg = new LiPacket(p->connection, data, C_DATA);
						m_ConnMan.send(pkg->connection, pkg->data, pkg->length);

						delete pkg;
						delete data;
					}

					// Send tasks to client
					client_state* c_state = m_State.m_States[p->connection];

					std::deque<client::Tasks*>::iterator i = m_State.m_OutQueue.begin();

					// Find a set the allocated client can handle
					while (i != m_State.m_OutQueue.end())
					{
						if ( ((*i)->batchhash()).compare(resp->batchhash()) == 0 )
						{
							// Found one!
							if ((*i)->task_size() <= c_state->send_tasks)
							{
								m_Kernel->Log(m_log_tag, "Sending %d tasks to %d"
									, (*i)->task_size()
									, p->connection->session_id
								);

								// Send all tasks and remove from outqueue
								LiPacket* pkg = new LiPacket(p->connection, (*i), C_TASKS);
								m_ConnMan.send(pkg->connection, pkg->data, pkg->length);

								delete pkg;
								delete (*i);
								i = m_State.m_OutQueue.erase(i);

								break; // Only send one set of tasks
							}
							else
							{
								// Split tasks into to parts, one to be kept in outqueue, and one to send
								client::Tasks* t = (*i);

								client::Tasks* send_t = new client::Tasks();
								client::Tasks* keep_t = new client::Tasks();
								
								send_t->set_batchhash(t->batchhash());
								keep_t->set_batchhash(t->batchhash());

								int j = 0;
								for ( ; j < c_state->send_tasks ; j++)
								{
									client::Tasks::Task* j_task = send_t->add_task();
									j_task->CopyFrom(t->task(j));
								}

								for ( ; j < t->task_size() ; j++)
								{
									client::Tasks::Task* j_task = keep_t->add_task();
									j_task->CopyFrom(t->task(j));
								}

								m_Kernel->Log(m_log_tag, "Sending %d tasks to %d"
									, send_t->task_size()
									, p->connection->session_id
								);
								
								LiPacket* pkg = new LiPacket(p->connection, send_t, C_TASKS);
								m_ConnMan.send(pkg->connection, pkg->data, pkg->length);

								i = m_State.m_OutQueue.erase(i);
								m_State.m_OutQueue.push_back(keep_t);

								delete send_t;
								delete pkg;
							}
						}
						else
							i++;
					}

					delete resp;
					break;
				}
				case C_DATA:
				{
					client::Data *data = (client::Data*)(p->unpack());

					Pxf::Util::String hash = data->batchhash();

					// Check that client has allocated the resource
					// TODO: And that stuff doesn't exist yet
					//if (count(m_State.m_Allocatees.begin(), m_State.m_Allocatees.end(), p->connection) != 1)
					//{
					//	m_Kernel->Log(m_log_tag, "Client has not been allocated!");
					//	delete data;
					//	break;
					//}

					Batch *b = new Batch();
					
					b->hash = (char*)Pxf::MemoryAllocate(hash.length() + 1);
					hash.copy(b->hash, hash.length());
					b->hash[hash.length()] = '\0';

					b->type = (BatchType)data->datatype();

					b->data_size = data->datasize();
					//b->data = (char*)Pxf::MemoryAllocate(b->data_size);
					b->data = data->data();
					/*
					Pxf::MemoryCopy(
						b->data,
						data->data().c_str(),
						b->data_size
					);*/

					b->timestamp = time(NULL);
					
					Pxf::Util::String str_address = data->returnaddress();
					b->return_address = (char*)Pxf::MemoryAllocate(str_address.length() + 1);
					str_address.copy(b->return_address, str_address.length());
					b->return_address[str_address.length()] = '\0';

					b->return_port = data->returnport();

					// Add batch to hashmap
					m_Batches[hash] = b;

					m_Kernel->Log(m_log_tag, "DATA from %d of type %d.", p->connection->session_id, b->type);

					delete data;

					break;
				}
				case C_TASKS:
				{
					client::Tasks *tasks = (client::Tasks*)(p->unpack());

					if (m_Batches.count(tasks->batchhash()) != 1)
					{
						m_Kernel->Log(m_log_tag, "Tasks sent without knowing batch data, dropping tasks!");
						// TODO: Send tasks to another client?
						delete tasks;
						break;
					}

					int num = tasks->task_size(); // Number of tasks

					// If there are no more tasks, continue to next package.
					if (!num)
					{
						m_Kernel->Log(m_log_tag, "Got tasks package, but containing zero tasks, dropping..");
						delete tasks;
						break;
					}

					Batch* b = m_Batches[tasks->batchhash()];
					int i = 0;

					m_Kernel->Log(m_log_tag, "Got %d tasks from [%d]", num, p->connection->session_id);

					// Fill internal queue
					// Will only work with lazy evaluation, is it standard? Dunno.. JONTE
					for ( ; (i < num) && m_TaskQueue->push(b->type, copy_task(tasks->task(i), b)); i++)
						;

					/*while (m_TaskQueue->push(b->type, copy_task(tasks->task(i), b)))
					{
						printf("PUSHED %d\n", i);
						if (i == num - 1)
							break;

						i++;
					}*/
					
					// Tasks up until (but not including) >i< have ben stored in internal queue
					m_Kernel->Log(m_log_tag, "Pushed %d tasks to internal queue.", i);

					if (i == num)
					{
						// All tasks have been pushed to internal queue, no forwarding needed
						m_Kernel->Log(m_log_tag, "No forwarding needed.");
						delete tasks;
						break;
					}

					// TODO: Current problems
					// - If clients are allocated, just send them tasks. How many?
					// - When requested clients of tracker, where to store tasks? And where to store 
					// 		how many tasks to send?
					// - Make the sending thread somehow send a message to client when internal queue
					// 		is empty. If external queue also is empty, send event to tracker.
					//
					// 	- How many can we send to? Request n more. Allocated + n == parts to send.
					// 	- Store how many tasks to send in state class? Only int required
					// 		But how about when more clients are requested?
					// 		Divide all pending tasks on new clients.. All are dispatched
					// 	- Allocated list in state class does not specify which batchhash the alocatee has
					//

					
					if (m_State.m_Clients.size() == 0)
					{
						// No clients known, request from tracker.
						int magic_number_1 = 6; // Max number of clients to send to
						int magic_number_2 = 10; // Preffered number of tasks per client
						request_nodes(num >= (magic_number_2 * magic_number_1) ? magic_number_1 : (num / magic_number_2) + 1);

						// Nothing more to do now, just wait for new clients, but store tasks in state first
						client::Tasks* n_tasks = new client::Tasks();
						for( ; i < num; i++)
						{
							client::Tasks::Task* n_task = n_tasks->add_task();
							n_task->CopyFrom(tasks->task(i));
						}
						n_tasks->set_batchhash(b->hash);

						m_State.m_OutQueue.push_back(n_tasks);

						delete tasks;
						break;
					}

					m_Kernel->Log(m_log_tag, "DUNNO WAT TO DOOO??! LOL");

					/*
					// Split the tasks in subgroups, copies data.
					Pxf::Util::Array<client::Tasks*> tasks = split_tasks(proto_tasks);

					delete proto_tasks;

					if (tasks.back()->task_size() > 0)
					{
						m_Kernel->Log(m_log_tag, "Pushing %d tasks from %d",
							tasks.back()->task_size(),
							p->connection->session_id
						);

						// Push a set of tasks to our queue
						push(tasks.back());
						
						// Remove the set we just used
						tasks.pop_back();

						// Forward the rest
						forward(tasks);

						// MASSIVE CODE GOES HERE?
					}
					*/
					break;
				}
				case T_NODES_RESPONSE:
				{
					tracker::NodesResponse *nodes = (tracker::NodesResponse*)(p->unpack());
					m_Kernel->Log(m_log_tag, "Allocation response from tracker, got %d nodes", nodes->nodes_size());

					for(int i=0; i < nodes->nodes_size(); i++)
					{
						m_Kernel->Log(m_log_tag, "node %d: %s:%d.", i, nodes->nodes(i).address().c_str(), nodes->nodes(i).port());

						Connection *new_node = m_ConnMan.new_connection(CLIENT);
						m_ConnMan.connect_connection(new_node, (char*)nodes->nodes(i).address().c_str(), nodes->nodes(i).port());
						
						client_state* state = new client_state;
						state->state = (ClientState)(WOK & W_HELLO);
						state->send_tasks = m_State.m_OutQueue.size() / (nodes->nodes_size() ? nodes->nodes_size() : 1);
						m_State.m_States[new_node] = state;

						client::Hello* hello = new client::Hello();
						hello->set_address(m_local_address);
						hello->set_port(m_client_port);
						hello->set_session_id(m_session_id);

						LiPacket* hello_pkg = new LiPacket(new_node, hello, C_HELLO);
						m_ConnMan.send(new_node, hello_pkg->data, hello_pkg->length);

						delete hello;
						delete hello_pkg;
					}

					delete nodes;
					break;
				}
				default:
					m_Kernel->Log(m_log_tag, "Unknown packet type: %d", p->message_type);
					break;
			}

			delete p;
			pack_iter = packets->erase(pack_iter);
		}

		//list_connections();

	}

	return 0;
}

// Request nodes from tracker
void Client::request_nodes(int _amount)
{
	tracker::NodesRequest* request = new tracker::NodesRequest();
	request->set_session_id(m_session_id);
	request->set_nodes(_amount);
																	   
	LiPacket* pkg = new LiPacket(m_tracker, request, T_NODES_REQUEST);
	m_ConnMan.send(pkg->connection, pkg->data, pkg->length);
																	   
	m_Kernel->Log(m_log_tag, "Requested %d nodes of tracker.", _amount);
																	   
	delete pkg;
	delete request;                                                   	
}

// Send allocation request to client
void Client::allocate_client(Connection* _c, Batch* _b, int _amount)
{
	client_state* state = m_State.m_States[_c];

	m_Kernel->Log(m_log_tag, "Sending ALLOCATE request to %d",
		_c->session_id);

	client::AllocateClient* alloc = new client::AllocateClient();
	alloc->set_batchhash(_b->hash);
	alloc->set_datatype(_b->type);
	alloc->set_amount(_amount);

	LiPacket* pkg = new LiPacket(_c, alloc, C_ALLOCATE);

	m_ConnMan.send(pkg->connection, pkg->data, pkg->length);

	delete alloc;
	delete pkg;

	state->state = WALLOC;
	state->batch = _b;
}

// Copy task data from protocol buffer to Lightning Task struct
Task* Client::copy_task(const client::Tasks::Task& _task, Batch* _batch)
{
	Task* t = new Task();

	t->batch = _batch;
	client::Tasks::Task* n_task = new client::Tasks::Task();
	n_task->CopyFrom(_task);
	t->task = n_task;
	return t;
}

// Initiate forwarding of tasks
/*
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
		m_ConnMan.send(pkg->connection, pkg->data, pkg->length);

		m_Kernel->Log(m_log_tag, "Requested %d nodes of tracker.", diff);

		delete pkg;
		delete request;
	}

	// Send what we can send
	Pxf::Util::Array<Connection*>::iterator i;
	Pxf::Util::Array<client::Tasks*>::iterator j;
	i = m_State.m_Allocated.begin();
	j = _tasks.begin();

	for ( ; ((i != m_State.m_Allocated.end()) && (j != _tasks.end())) ;  i++, j++ )
	{
		LiPacket *pkg = new LiPacket(*i, *j, C_TASKS);
		m_Kernel->Log(m_log_tag, "Sending tasks to client %d\n", (*i)->session_id);
		m_ConnMan.send(pkg->connection, pkg->data, pkg->length);
		delete pkg;
	}
	// TODO: Delete the tasks that just have been sent
	
	// Check which iterator finished first
	if ( i == m_State.m_Allocated.end())
	{
		m_Kernel->Log(m_log_tag, "Not all tasks could be sent, awaiting tracker response\n");
		// Not all tasks could be forwarded, store the rest in the state
		for ( ; j != _tasks.end(); j++)
		{
			m_State.m_OutQueue.push_back((*j));
			m_Kernel->Log(m_log_tag, "Push task to OutQueue\n");
		}
	}
}*/


/*
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
*/

// Split the tasks in chunks
Pxf::Util::Array<client::Tasks*> Client::split_tasks(client::Tasks* _tasks)
{
	//int parts = 2; // TODO: Smarter stuff!
	int parts = _tasks->task_size() < 2 ? 1 : 2;
	//int n = _tasks->task_size() / parts;
	Pxf::Util::Array<client::Tasks*> tasks;

	// Create new tasks sets
	for (int i = 0; i < parts; i++)
	{
		client::Tasks* t = new client::Tasks();
		t->set_batchhash(_tasks->batchhash());
		tasks.push_back(t);
	}

	for (int i = 0; i < _tasks->task_size(); i++) {
		client::Tasks_Task* t = tasks[i%parts]->add_task();
		t->CopyFrom(_tasks->task(i));
	}

	return tasks;
}

void Client::attach(LightningClient* _client)
{
		_client->initialize(m_TaskQueue, m_ResultQueue);
		_client->run_noblock();
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
	Pxf::Util::Array<Packet*> *packets = m_ConnMan.recv_packets(5000);
	if (packets->size() == 0)
	{
		m_Kernel->Log(m_net_tag, "Connection to tracker at %s timed out.", c->target_address);
		return false;
	}

	LiPacket* pkg = new LiPacket(packets->front());

	tracker::HelloToClient *hello_client = (tracker::HelloToClient*)pkg->unpack();

	m_session_id = hello_client->session_id();

	delete pkg;
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

	pkg = new LiPacket(c, hello_tracker, T_HELLO_TRACKER);

	m_ConnMan.send(c, pkg->data, pkg->length);
	m_Kernel->Log(m_log_tag, "Connection to tracker on socket %d terminated.", c->socket);
	m_ConnMan.remove_connection(c);
	delete pkg;

	packets = m_ConnMan.recv_packets(5000);

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

