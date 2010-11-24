#if defined(CONF_FAMILY_UNIX)
#include <sys/socket.h>
#endif

#include "connectionmanager.h"

Connection::Connection(ConnectionType _type, int _id)
	: socket(0)
	, buffer(NULL)
	, buffer_cur(NULL)
	, buffer_size(0)
	, id(_id)
	, session_id(0)
	, type(_type)
{}

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
	int sent = 0, left = _length;

	do {
		sent = send(_connection->socket, _msg + sent, left);
		left = left - sent;
	} while (sent < left);

	return (sent != -1);
}

bool ConnectionManager::send(int _id, char *_msg, int _length, bool _is_session_id)
{
	Pxf::Util::Array<struct Connection*>::iterator i;

	// Copypasta below, but faster than checking every iteration
	if _is_session_id
	{
		for (i = m_Connections; i < m_Connections.end(); i++) {
			if ((*i)->session_id == _id) return send(*i, _msg, _length);
		}
	}
	else
	{
		for (i = m_Connections; i < m_Connections.end(); i++) {
			if ((*i)->id == _id) return send(*i, _msg, _length);
		}
	}

	return false;
}
