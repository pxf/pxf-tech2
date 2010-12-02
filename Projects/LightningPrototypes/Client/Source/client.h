#include <Pxf/Util/Array.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Memory.h>
#include <zthread/ThreadedExecutor.h>

#include "connectionmanager.h"
#include "lightning.h"
#include "iomodule.h"

#ifndef _CLIENT_H_
#define _CLIENT_H_

class Client
{
	public:
		int m_queue_free;
		Pxf::Util::Array<int> m_TaskQueue;
		int m_session_id;
		char *m_tracker_address;
		int m_tracker_port;
		char *m_local_address;
		int m_local_port;

		Client(
			const char *_tracker_address,
			int _tracker_port,
			const char *_local_address,
			int _local_port
		);
		~Client();
		int run();

	private:
		ConnectionManager m_ConnMan;
		Pxf::Kernel* m_Kernel;

		int m_net_tag;	
		int m_log_tag;
		void ping(Connection *_c, int _timestamp);
		bool connect_tracker();
};

#endif

