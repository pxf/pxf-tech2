#include <Pxf/Base/String.h>

#include <zmq.hpp>

#include "client.h"
#include "lightning.h"
#include "trackerclient.pb.h"

/* Connects to the tracker at the specified endpoint */
int connect_tracker(void* socket, const char* endpoint)
{
	// If we are unable to connect, a NULL pointer is returned.
	// TODO: Report error properly
	if(zmq_connect(socket, endpoint) != 0) return NULL;

	int msg_type = INIT_HELLO;
	zmq_msg_t init_hello;
	zmq_msg_init_data(&init_hello, &msg_type, sizeof(msg_type), NULL, NULL);

	zmq_send(socket, &init_hello, 0);

	message* credentials = recv_message(socket);

	if(credentials->type != HELLO_TO_CLIENT)
	{
		// Message is not what is expected
		// TODO: Report error properly
		return NULL;
	}

	trackerclient::HelloToClient* hello = (trackerclient::HelloToClient*)(credentials->protobuf_data);

	int session_id = Pxf::StringToInteger(hello->session_id().c_str());

	delete(credentials);

	// Assign client->address it's corresponding pointer

	return int;
}


