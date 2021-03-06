#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <Pxf/Util/Array.h>
#include <Pxf/Util/String.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Memory.h>
#include <zthread/ThreadedExecutor.h>
#include <zthread/BlockingQueue.h>

#include "connectionmanager.h"
#include "lightning.h"
#include "state.h"
#include "BlockingTaskQueue.h"
#include "RaytracerClient.h"

class Client
{
	public:
		typedef ZThread::BlockingQueue<TaskResult*, ZThread::FastMutex> TaskResultQueue;

		int m_session_id;
		char *m_tracker_address;
		int m_tracker_port;
		char *m_local_address;
		int m_local_port;
		int m_client_port;

		Client(const Client& blah) {}

		Client(
			const char *_tracker_address,
			int _tracker_port,
			const char *_local_address,
			int _local_port,
			int _client_port,
			int _max_clients = 6,
			int _preferred_tasks = 10
		);
		~Client();
		int run();

		void attach(LightningClient* _client);
		void signal_availability(int _amount);
		State m_State;
		
		// made these three public, needed inside RaytracerClient
		BlockingTaskQueue<Task*>* m_TaskQueue;
		Pxf::Util::Map<Pxf::Util::String, Batch*> m_Batches;
		Task* copy_task(const client::Tasks::Task& _task, Batch* _batch);

	private:
		ConnectionManager m_ConnMan;
		Pxf::Kernel* m_Kernel;
		
		Connection* m_tracker;

		ZThread::ThreadedExecutor* m_ThreadedExecutor;
		TaskResultQueue* m_ResultQueue;


		int m_net_tag;	
		int m_log_tag;
		int last_batch_check;

		int m_max_clients;
		int m_preferred_tasks;
		
		void ping(Connection *_c, int _timestamp);
		bool connect_tracker();
		Pxf::Util::Array<client::Tasks*> split_tasks(client::Tasks*);
		void push(client::Tasks* _tasks);
		void forward(Pxf::Util::Array<client::Tasks*> _tasks);
		void list_connections();
		

		Connection* find_connection(int _id);
		void request_nodes(int _amount);
		void allocate_client(Connection* _c, Batch* _b, int _amount);

};

#endif

