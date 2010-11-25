#if defined(CONF_FAMILY_UNIX)
#include <sys/socket.h>
#endif


#include "connectionmanager.h"

#define MAX_SEND_ITERATIONS 20

Connection::Connection(ConnectionType _type, int _id)
	: socket(0)
	, buffer(NULL)
	, buffer_cur(NULL)
	, buffer_size(0)
	, id(_id)
	, session_id(0)
	, type(_type)
{}

Connection::~Connection()
{
	if (buffer_size != 0)
		delete buffer;
	// TODO: Close the socket.
}

Packet::~Packet()
{
	delete data;
}

Connection *ConnectionManager::new_connection(ConnectionType _type)
{
	Connection *connection = new Connection(_type, m_NextId++);

	m_Connections.push_back(connection);

	return connection;
}

bool ConnectionManager::bind_connection(Connection *_connection, char *_address, int _port)
{
	// TODO: Check whether the connection exists in our m_Connection or not.

	return false;
}

bool ConnectionManager::connect_connection(Connection *_connection, char *_address, int _port)
{
	return false;
}

Connection *ConnectionManager::get_connection(int _id, bool _is_session_id)
{
	return NULL;
}

Packet *ConnectionManager::recv()
{
	return NULL;
}

bool ConnectionManager::send(Connection *_connection, char *_msg, int _length)
{	
	int i = 0, left = _length;

	do {
		left = left - send(_connection->socket, _msg + sent, left);
		i++;
	} while ((left > 0) && (i < MAX_SEND_ITERATIONS));

	return (left == 0);
}

bool ConnectionManager::send(int _id, char *_msg, int _length, bool _is_session_id)
{
	Connection *c = get_connection(_id, _is_session_id);
	return (c == NULL) ? false : send(c, _msg, _length);

}

