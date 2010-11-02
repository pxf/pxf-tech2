#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Timer.h>
#include <Pxf/Base/Logger.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Random.h>
#include <Pxf/Base/Memory.h>
#include <ctime>
#include <cstdio>
#include <cstdlib>

#include <zmq.h>

using namespace Pxf;

int main(int argc, char* argv[])
{
	Pxf::RandSetSeed(time(NULL));
	Kernel* kernel = Pxf::Kernel::GetInstance();
	
	void* context = zmq_init(1);
	
	/* Client */
	if (argc > 1 && StringCompareI(argv[1], "client") == 0)
	{
		void *requester = zmq_socket (context, ZMQ_REQ);
		
		zmq_connect (requester, "tcp://localhost:50001");
		unsigned int request_nbr;
		for (request_nbr = 0; request_nbr != 10000; request_nbr++)
		{
			zmq_msg_t request;
			zmq_msg_init_data (&request
							  , "Hello"
							  , 6
							  , 0
							  , 0);
			zmq_send (requester, &request, 0);
			zmq_msg_close (&request);

			zmq_msg_t reply;
			zmq_msg_init (&reply);
			zmq_recv (requester, &reply, 0);
			printf ("Received reply %d: [%s]\n", request_nbr, (char *) zmq_msg_data (&reply));
			zmq_msg_close (&reply);
		}
	zmq_close (requester);
	
	}
	/* Server */
	else if (argc > 1 && StringCompareI(argv[1], "server") == 0)
	{
		void* responder = zmq_socket(context, ZMQ_REP);
		zmq_bind(responder, "tcp://*:50001");
		
		while(1)
		{
			// Recieve data
			zmq_msg_t request;
			zmq_msg_init(&request);
			zmq_recv(responder, &request, 0);
			printf("Received request: %s\n", zmq_msg_data(&request));
			zmq_msg_close(&request);
			
			// Respond
			zmq_msg_t reply;
			zmq_msg_init_size (&reply, 6);
			MemoryCopy((void *) zmq_msg_data (&reply), "World", 6);
			zmq_send (responder, &reply, 0);
			zmq_msg_close (&reply);
		}
		
	}
	
	zmq_term(context);
	
	return 0;
}

