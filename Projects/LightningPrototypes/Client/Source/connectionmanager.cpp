#if defined(CONF_FAMILY_UNIX)
#include <sys/socket.h>
#endif

#include "connectionmanager.h"

Connection *ConnectionManager::new_connection(ConnectionType _type)
{
	return NULL;
}

bool ConnectionManager::bind_connection(Connection *_connection, char *_address, int _port)
{
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

bool ConnectionManager::Send(Connection *_connection, char *_msg, int _length)
{
	// Blocking as hell
	if (send(_connection->socket, _msg, length, 0) == -1) return false;
}

bool ConnectionManager::send(int _id, char *_msg, int _length, bool _is_session_id)
{
	return false;
}
