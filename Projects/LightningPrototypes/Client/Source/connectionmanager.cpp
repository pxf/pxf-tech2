#if defined(CONF_FAMILY_UNIX)
#include <sys/socket.h>
#endif

#include "connectionmanager.h"

bool ConnectionManager::Send(Connection *_connection, char *_msg, int _length)
{
	// Blocking as hell
	if (send(_connection->socket, _msg, length, 0) == -1) return false;
}


