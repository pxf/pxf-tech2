#include <stdio.h>

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>

#include "client.h"
#include "connectionmanager.h"

using namespace Pxf;
using namespace Util;

int main(int argc, char *argv[])
{
	Client client = Client("users.mkeyd.net", 50000, "tcp://193.11.200.91:500001");
	client.run();

	return(0);
}

