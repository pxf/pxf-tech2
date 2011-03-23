#ifndef _CONNECTIONMANAGER__H_
#define _CONNECTIONMANAGER__H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include <Pxf/Kernel.h>
#include <Pxf/Base/Logger.h>
#include <Pxf/Base/Config.h>
#include <Pxf/Base/Memory.h>
#include <Pxf/Util/Array.h>
#include <Pxf/Util/Map.h>


#if defined(CONF_FAMILY_UNIX)
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/select.h>
	#include <unistd.h>
	#include <netdb.h>
	#include <arpa/inet.h>
#endif

#if defined(CONF_FAMILY_WINDOWS)
	#define WIN32_LEAN_AND_MEAN
	#define VC_EXTRALEAN
	#include <WinSock2.h>
	#include <ws2tcpip.h>
	#pragma comment(lib,"ws2_32.lib")
#endif


enum ConnectionType {
	CLIENT, TRACKER, INTERNAL, ORIGIN
};

struct Connection {
	Connection(ConnectionType _type, int _id);
	~Connection();

	int socket;
	char *buffer;
	char *buffer_cur;
	char target_address[16]; // 16 = length of inet6
	int target_port;
	int buffer_size;
	int id; // locally set for connections.
	int session_id; // globally set by tracker.
	bool bound;
	time_t timestamp;
	ConnectionType type;
};

struct Packet {
	Packet(Connection *_connection, int _length, char *_data)
		: connection(_connection)
		, length(_length)
		, data(_data) // TODO: Allocate new memory and copy?
	{}
	Packet()
		: connection(0)
		, length(0)
		, data(0)
	{};

	virtual ~Packet();

	Connection *connection;
	int length;
	char *data;
};

class ConnectionManager
{
	private:
		Pxf::Util::Array<Packet*> *m_Packets;
		Pxf::Util::Map<int, struct Connection *> m_socketfdToConnection;
		Pxf::Kernel* m_Kernel;

		int m_NextId;
		int m_max_socketfd;
		fd_set m_read_sockets;
		fd_set m_error_sockets;

		void clear_connbuf(Connection *_connection);
		void set_fdset();

	public:
		friend class Client;
		int m_log_tag;
		Pxf::Util::Array<struct Connection *> m_Connections;

		ConnectionManager();
		ConnectionManager(Pxf::Util::Array<Packet*> *_packets);

		Connection *new_connection(ConnectionType _type);
		bool bind_connection(Connection *_connection, char *_address, int _port);
		bool connect_connection(Connection *_connection, char *_address, int _port);
		void add_incoming_connection(int _socket, ConnectionType _type);
		bool remove_connection(Connection *_connection);
		
		Connection *get_connection(int _id, bool _is_session_id = false);
		
		Pxf::Util::Array<Packet*> *recv_packets(int _timeout = 0);

		bool send(Connection *_connection, char *_msg, int _length);
		bool send(int _id, char *_msg, int _length, bool _is_session_id = false);
		bool send(Packet *packet);

		void set_highest_fd();
};

#endif  /* _CONNECTIONMANAGER__H_ */
