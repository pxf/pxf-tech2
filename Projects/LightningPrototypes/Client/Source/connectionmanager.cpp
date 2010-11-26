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

	m_Kernel = Pxf::Kernel::GetInstance();
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
	//hints.ai_flags = AI_PASSIVE;

	sprintf(port, "%d", _port);

	if ((status = getaddrinfo(_address, port, &hints, &res)) != 0)
	{
		fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(status));
		return false;
	}

	sck = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (connect(sck, res->ai_addr, res->ai_addrlen) != 0)
	{
		fprintf(stderr, "failed to connect to %s:%s.\n", _address, port);
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

Pxf::Util::Array<Packet*> *ConnectionManager::recv_packets(int _timeout)
{
	Pxf::Util::Array<Packet*>* packets = new Pxf::Util::Array<Packet*>();
	struct timeval timeout;

	timeout.tv_sec = _timeout/1000;
	timeout.tv_usec = (_timeout%1000)*1000;

	// TODO: Log error
	if (select(m_max_socketfd, &m_read_sockets, NULL, NULL, &timeout) == -1)
	{
		m_Kernel->Log(0, "Unable to call select().");
		return NULL;
	}

	m_Kernel->Log(0, "Passed select()");

	Connection *c;
	for (int i=0; i <= m_max_socketfd; i++) 
	{
		if (FD_ISSET(i, &m_read_sockets))
		{
			c = m_socketfdToConnection[i];
			if (c->bound)
			{
				int new_connection_fd;
				struct sockaddr remoteaddr;
				socklen_t addrlen = sizeof(&remoteaddr);
				new_connection_fd = accept(i, &remoteaddr, &addrlen);

				m_Kernel->Log(0, "Incoming connection from ?"); // TODO: Print address

				if (c->type == TRACKER)
				{
					// Check that the tracker isn't connected already
					Pxf::Util::Array<struct Connection*>::iterator j;
					
					bool tracker_connected = false;
					for (j = m_Connections.begin(); j != m_Connections.end(); j++) {
						if ((*j)->type == TRACKER) {
							tracker_connected = true;
							break;
						}
					}

					if (tracker_connected) {
						m_Kernel->Log(0, "Tracker is trying to connect, but is already connected!");
						continue;
					}
					
					m_Kernel->Log(0, "Tracker connected...");
					add_incoming_connection(new_connection_fd, TRACKER);
				}
				else if (c->type == CLIENT)
				{
					// TODO: Tedious check that client already isn't connected
					add_incoming_connection(new_connection_fd, CLIENT);
				}
			} else { // if (c->bound)
				int recv_bytes;

				if (c->buffer_size == 0)
				{
					// New message, read message length
					recv_bytes = recv(c->socket, (void*)(&(c->buffer_size)), sizeof(c->buffer_size), 0);
					if (recv_bytes == 0)
					{
						// TODO: Terminate connection
						c->buffer_size = 0;
						continue;
					}

					c->buffer = (char*)Pxf::MemoryAllocate(c->buffer_size);
					
					recv_bytes = recv(c->socket, (void*)(c->buffer), c->buffer_size, 0);

					if (recv_bytes == c->buffer_size)
					{
						// Entire message recieved, continue to next available socket
						Packet* p = new Packet(c, c->buffer_size, c->buffer);
						packets->push_back(p);
						clear_connbuf(c);
						continue;
					} else {
						// More to recieve next run..
						c->buffer_cur = c->buffer + recv_bytes;
						continue;					
					}
				}
				else
				{
					// Part of message in transit
					int size_left = c->buffer_size - (c->buffer_cur-c->buffer);
					recv_bytes = recv(c->socket, c->buffer_cur, size_left, 0);

					if (recv_bytes == 0)
					{
						// TODO: Terminate connection
						Pxf::MemoryFree(c->buffer);
						clear_connbuf(c);
						continue;
					}

					if (recv_bytes == size_left)
					{
						// Entire message recieved, continue...
						Packet* p = new Packet(c, c->buffer_size, c->buffer);
						packets->push_back(p);
						clear_connbuf(c);
						continue;        
					} else {
						// Not done yet
						c->buffer_cur += recv_bytes;
					}
				} // New or old message
			} // if (c->bound)
		} // if (FD_ISSET)
	}

	return packets;
}

bool ConnectionManager::send(Connection *_connection, char *_msg, int _length)
{	
	int sent, i=0, offset=0;

	// Transmit the length of the message
	sent = ::send(_connection->socket, &_length, sizeof(_length), 0);
	if (sent <= 0)
	{
		m_Kernel->Log(0, "Error while sending to %s: %s:", c->target_address, strerror(errno));
		return false;
	}

	do {
		sent = ::send(_connection->socket, _msg+offset, _length-offset, 0);
		if (sent <= 0)
		{
			m_Kernel->Log(0, "Error while sending to %s: %s:", c->target_address, strerror(errno));
			return false;
		}
		offset += sent;
		i++;
	} while ((offset < _length) && (i < MAX_SEND_ITERATIONS));
	
	m_Kernel->Log(0, "Sent %d bytes of data to %s", _length+sizeof(_length), c->target_address);

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

/* Resets the buffer variables and pointers. Will not deallocate any data. */
void inline ConnectionManager::clear_connbuf(Connection *_connection)
{
	_connection->buffer = NULL;
	_connection->buffer_cur = NULL;
	_connection->buffer_size = 0;
}

