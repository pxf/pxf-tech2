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
	// TODO: Remove _address... is it necessary?
	// TODO: Check whether the connection exists in our m_Connection or not.
	int status, sck;
	struct addrinfo hints, *res;
	char port[7];
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((status = getaddrinfo(NULL, port, &hints, &res)) != 0)
	{
		fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(status));
		return false;
	}

	sck = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	sprintf(port, "%d\0", _port);

	if (bind(sck, res->ai_addr, res->ai_addrlen) != 0)
	{
		fprintf(stderr, "unable to bind.");
		return false;
	}

	return false;
}

bool ConnectionManager::connect_connection(Connection *_connection, char *_address, int _port)
{
	// Create the socket.
	int status, sck;
	struct addrinfo hints, *res;
	char port[7];

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	sprintf(port, "%d\0", _port);

	if ((status == getaddrinfo(_address, port, &hints, &res)) != 0)
	{
		fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(status));
		return false;
	}

	sck = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (connect(sck, res->ai_addr, res->ai_addrlen) != 0)
	{
		fprintf(stderr, "failed to connect.\n");
		return false;
	}

	_connection->socket = sck;

	return true;
}

Connection *ConnectionManager::get_connection(int _id, bool _is_session_id)
{
	Pxf::Util::Array<struct Connection*>::iterator i;

	for (i = m_Connections.begin(); i < m_Connections.end(); i++) {
		if (((_is_session_id) ? (*i)->session_id : (*i)->id) == _id)
			return (*i);
	}
	
	return NULL;
}

Packet *ConnectionManager::recv()
{
	return NULL;
}

bool ConnectionManager::send(Connection *_connection, char *_msg, int _length)
{	
	int i=0, offset=0;

	do {
		offset += send(_connection->socket, _msg+offset, _length-offset);
		i++;
	} while ((offset < _length) && (i < MAX_SEND_ITERATIONS));

	return (offset <= _length);
}

bool ConnectionManager::send(int _id, char *_msg, int _length, bool _is_session_id)
{
	Connection *c = get_connection(_id, _is_session_id);
	return (c == NULL) ? false : send(c, _msg, _length);

}

