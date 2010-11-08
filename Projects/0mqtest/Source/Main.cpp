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

#include <zmq.hpp>

using namespace Pxf;

int main(int argc, char* argv[])
{
	Pxf::RandSetSeed(time(NULL));
	Kernel* kernel = Pxf::Kernel::GetInstance();
	
	zmq::context_t context(1);
	
	/* Client */
	if (argc > 1 && StringCompareI(argv[1], "client") == 0)
	{
		zmq::socket_t socket(context, ZMQ_REQ);
		socket.connect("tcp://users.mkeyd.net:50001");
		unsigned int request_nbr;
		for (request_nbr = 0; request_nbr != 10000; request_nbr++)
		{
			zmq::message_t request(6);
			MemoryCopy((void*) request.data(), "Hello", 6);
			socket.send(request);

			zmq::message_t reply;
			socket.recv(&reply);
			printf ("Received reply %d: [%s]\n", request_nbr, (char*)reply.data());
		}
	}
	/* Server */
	else if (argc > 1 && StringCompareI(argv[1], "server") == 0)
	{
		zmq::socket_t socket(context, ZMQ_REP);
		socket.bind("tcp://*:50001");
		
		while(1)
		{
			// Recieve data
			zmq::message_t request;
			socket.recv(&request);
			printf("Received request: %s\n", request.data());
			
			// Respond
			zmq::message_t reply(6);
			MemoryCopy((void *) reply.data(), "World", 6);
			socket.send(reply);
		}
	}
	return 0;
}

