#include <stdio.h>

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>

#include "lightning.h"
#include "client.h"

using namespace Pxf;
using namespace Util;

int main(int argc, char *argv[])
{
	Client client = Client("tcp://users.mkeyd.net:50000", "tcp://193.11.200.91:500001");
	printf("Connecting to tracker at %s\n", client.tracker_address);
	if (client.connect_tracker(client.tracker_address) == -1 )
	{
		printf("Could not connect to tracker at %s.\n", client.tracker_address);
		return(-1);
	}
	printf("session_id:%s\n",client.session_id);


	return(0);
}

