#include <stdio.h>

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>

#include "client.h"
#include "connectionmanager.h"

using namespace Pxf;
using namespace Util;

int main(int argc, char *argv[])
{
	Client client = Client("users.mkeyd.net", 50000, "192.168.1.100" ,50001);
	client.run();

	return(0);
}

