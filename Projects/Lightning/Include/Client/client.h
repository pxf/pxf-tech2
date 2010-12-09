#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <Pxf/Util/Array.h>
#include <Pxf/Util/String.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Memory.h>
#include <zthread/ThreadedExecutor.h>

#include "connectionmanager.h"
#include "lightning.h"
#include "state.h"
#include "BlockingTaskQueue.h"

class Client
{
	public:
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
			int _client_port
		);
		~Client();
		int run();

	private:
		ConnectionManager m_ConnMan;
		State m_State;
		Pxf::Kernel* m_Kernel;
		Pxf::Util::Map<Pxf::Util::String, Batch*> m_Batches;
		BlockingTaskQueue<Task*>* m_TaskQueue;

		int m_queue_free;
		int m_net_tag;	
		int m_log_tag;
		void ping(Connection *_c, int _timestamp);
		bool connect_tracker();

		int last_batch_check;
};

#endif

