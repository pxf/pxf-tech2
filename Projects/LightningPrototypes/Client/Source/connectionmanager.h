#ifndef _CONNECTIONMANAGER__H_
#define _CONNECTIONMANAGER__H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Pxf/Util/Array.h>
#include <Pxf/Base/Config.h>

#if defined(CONF_FAMILY_UNIX)
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/select.h>
	#include <unistd.h>
	#include <netdb.h>
#endif

#if defined(CONF_FAMILY_WINDOWS)
	#include <winsock.h>
#endif


enum ConnectionType {
	CLIENT, TRACKER, INTERNAL
};

struct Connection {
	Connection(ConnectionType _type, int _id);
	~Connection();

	int socket;
	char *buffer;
	char *buffer_cur;
	int buffer_size;
	int id; // locally set for connections.
	int session_id; // globally set by tracker.
	bool bound;
	ConnectionType type;
};

struct Packet {
	Packet(Connection *_connection, int _length, char *_data)
		: connection(_connection)
		, length(_length)
		, data(_data) // TODO: Allocate new memory and copy?
	{}
	~Packet();

	Connection *connection;
	int length;
	char *data;
};

class ConnectionManager
{
	private:
		Pxf::Util::Array<struct Connection *> m_Connections;

		int m_NextId;
		int m_max_socketfd;
		fd_set m_read_sockets;

	public:
		ConnectionManager();

		Connection *new_connection(ConnectionType _type);
		bool bind_connection(Connection *_connection, char *_address, int _port);
		bool connect_connection(Connection *_connection, char *_address, int _port);
		void add_incoming_connection(int _socket, ConnectionType _type);
		bool remove_connection(Connection *_connection);
		
		Connection *get_connection(int _id, bool _is_session_id = false);
		
		Packet *recv();

		bool send(Connection *_connection, char *_msg, int _length);
		bool send(int _id, char *_msg, int _length, bool _is_session_id = false);
};

#endif  /* _CONNECTIONMANAGER__H_ */
