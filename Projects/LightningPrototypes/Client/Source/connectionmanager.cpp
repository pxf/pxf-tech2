#include "connectionmanager.h"

#define MAX_SEND_ITERATIONS 20

Connection::Connection(ConnectionType _type, int _id)
	: socket(-1)
	, buffer(NULL)
	, buffer_cur(NULL)
	, buffer_size(0)
	, id(_id)
	, session_id(0)
	, type(_type)
	, bound(false)
{}

Connection::~Connection()
{
	if (buffer_size != 0)
		delete buffer;

	close(socket);
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

ConnectionManager::ConnectionManager()
{
	m_NextId = 1;
	m_max_socketfd = 0;
	FD_ZERO(&m_read_sockets);
}

void ConnectionManager::add_incoming_connection(int _socket, ConnectionType _type)
{
	Connection *connection = new_connection(_type);

	connection->socket = _socket;
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

	_connection->socket = sck;
	_connection->bound = true;

	return false;
}

bool ConnectionManager::remove_connection(Connection *_connection)
{
	bool have_socket = (_connection->socket != -1);

	if (have_socket)
		FD_CLR(_connection->socket, &m_read_sockets);

	delete _connection;

	if (have_socket)
		set_highest_fd();

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

	FD_SET(sck, &m_read_sockets);
	m_socketfdToConnection.insert(std::make_pair(sck, _connection));
	m_max_socketfd = (sck > m_max_socketfd) ? sck : m_max_socketfd;

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
	struct timeval timeout;

	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	// TODO: Log error
	if (select(m_max_socketfd, &m_read_sockets, NULL, NULL, &timeout) == -1) return NULL;

	Connection *c;
	for (int i=0; i <= m_max_socketfd; i++) 
	{
		if (FD_ISSET(i, &m_read_sockets))
		{
			c = m_socketfdToConnection[i];
			
		}
	}

}

bool ConnectionManager::send(Connection *_connection, char *_msg, int _length)
{	
	int i=0, offset=0;

	do {
		offset += send(_connection->socket, _msg+offset, _length-offset, 0);
		i++;
	} while ((offset < _length) && (i < MAX_SEND_ITERATIONS));

	return (offset <= _length);
}

bool ConnectionManager::send(int _id, char *_msg, int _length, bool _is_session_id)
{
	Connection *c = get_connection(_id, _is_session_id);
	return (c == NULL) ? false : send(c, _msg, _length);

}

void ConnectionManager::set_highest_fd()
{
	Pxf::Util::Array<struct Connection*>::iterator i;
	int max;

	for (i = m_Connections.begin(); i < m_Connections.end(); i++) {
		if (max < (*i)->socket)
			max = (*i)->socket;
	}

	m_max_socketfd = max;
}

