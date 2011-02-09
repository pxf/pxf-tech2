#include <stdio.h>

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>

#include "client.h"
#include "connectionmanager.h"

using namespace Pxf;
using namespace Util;

int main(int argc, char *argv[])
{
	//Client client = Client("users.mkeyd.net", 50000, "129.16.74.87", 50001);
	//Client client = Client("127.0.0.1", 50000, "127.0.0.1", 50001, 50002);
	Client client = Client("dorsin.csbnet.se", 50000, "dorsin.csbnet.se", 50001, 50002);

    RaytracerClient *t_Raytracerclient;
	t_Raytracerclient = new RaytracerClient(Kernel::GetInstance());

    client.attach(t_Raytracerclient);
    
	client.run();

	return(0);
}

