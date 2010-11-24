#ifndef _CONNECTIONMANAGER__H_
#define _CONNECTIONMANAGER__H_

#include <Pxf/Util/Array.h>

enum ConnectionType {
	CLIENT, TRACKER, INERNAL
};

struct Connection {
	int socket;
	char *buffer;
	int id; // locally set for connections.
	int session_id; // globally set by tracker.
	ConnectionType type;
};

class ConnectionManager
{
	private:
		Pxf::Util::Array<struct Connection *> m_Connections;

	public:
		int NewConnection();
		bool BindConnection(int _id, char *_address, int _port);
		bool ConnectConnection(int _id, char *_address, int _port);
		
		int session_id2id(int _session_id);
		int id2session_id(int _id);
		int id2socket(int _id);
		
		struct Connection *Select();

		bool Send(struct Connection *_connection, char *_msg, int _length);
		//bool Send(int _session_id, _msg, _length);
		bool Send(int _id, char *_msg, int _length);
};

#endif  /* _CONNECTIONMANAGER__H_ */
