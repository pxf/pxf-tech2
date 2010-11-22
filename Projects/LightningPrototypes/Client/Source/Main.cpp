#include <stdio.h>

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>

#include "lightning.h"
#include "client.h"

using namespace Pxf;
using namespace Util;

int main(int argc, char *argv[])
{
	Client client = Client("tcp://users.mkeyd.net:50000", "aoeu");
	printf("Connecting to tracker...\n");
	if (client.connect_tracker(client.tracker_address) == -1 )
	{
		printf("Could not connect to tracker at %s.\n", client.tracker_address);
		return(-1);
	}
	printf("session_id:%i\n",client.session_id);

	return(0);
}

