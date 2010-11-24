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

Packet *ConnectionManager::recv()
{
	return NULL;
}

bool ConnectionManager::send(Connection *_connection, char *_msg, int _length)
{
	return false;
}

bool ConnectionManager::send(int _id, char *_msg, int _length, bool _is_session_id)
{
	return false;
}
