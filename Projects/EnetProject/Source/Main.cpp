//#include <Pxf/SampleModule/SampleModule.h>
#include <stdio.h>
#include <string.h>

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

#include <Pxf/Audio/AudioDevice.h>

#include <Pxf/Input/InputDevice.h>
#include <Pxf/Input/InputDefs.h>

#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Graphics/Window.h>
#include <Pxf/Graphics/WindowSpecifications.h>

#include <Pxf/Base/Hash.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Memory.h>
#include <Pxf/Base/Random.h>

#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/ResourceLoader.h>
#include <Pxf/Resource/Image.h>
#include <Pxf/Resource/Blob.h>

//#include <Pxf/Network/NetworkDevice.h>

#include <Pxf/Modules/net/ENetServer.h>

#include <enet/enet.h>

#include <ctime>

using namespace Pxf;


int main(int argv, char *argc[])
{
/*    Pxf::RandSetSeed(time(NULL));
    Kernel* kernel = Pxf::Kernel::GetInstance();

	 kernel->RegisterModule("pri", Pxf::System::SYSTEM_TYPE_GRAPHICSDEVICE | Pxf::System::SYSTEM_TYPE_INPUTDEVICE, true);
    kernel->RegisterModule("img", Pxf::System::SYSTEM_TYPE_RESOURCE_LOADER, true);
    kernel->DumpAvailableModules();

    Graphics::GraphicsDevice* gfx = kernel->GetGraphicsDevice();
    Audio::AudioDevice* snd = kernel->GetAudioDevice();
    Input::InputDevice* inp = kernel->GetInputDevice();
    Resource::ResourceManager* res = kernel->GetResourceManager();
    Resource::Image* img = res->Acquire<Resource::Image>("test.png");

    res->DumpResourceLoaders();

    snd->Play(2);

    Resource::BlobLoader* loader = res->FindResourceLoader<Resource::BlobLoader>("blob");
    Resource::Blob* blob = loader->CreateFrom("aoeu", 5);
	 */

    /*
    // Resource::BinaryFile to complement? Resource::Blob, binary large object
    Resource::TextFile* shadersrc = res->Acquire<Resource::TextFile>("fiddle.glsl");
    Graphics::Shader* shader = video->CreateShader(Resource::TextFile* _source);
    shader->SetBlahBlah();
    */

/*
    Graphics::WindowSpecifications spec;
    spec.Width = 800;
    spec.Height = 600;
    spec.ColorBits = 24;
    spec.AlphaBits = 8;
    spec.DepthBits = 8;
    spec.StencilBits = 0;
    spec.FSAASamples = 0;
    spec.Fullscreen = false;
    spec.Resizeable = false;
    spec.VerticalSync = false;
    
    Graphics::Window* win = gfx->OpenWindow(&spec);
	 */

	 if (enet_initialize() != 0)
	 {
	 	Message("Enet", "Couldn't init Enet.");
	 }

	 int isserver = (argv > 1 && !strcmp(argc[1], "server"));

	 if (isserver)
	 {
	 	Message("Server", "Setting up server...");
		ENetAddress address;
		ENetHost *server;

		address.host = ENET_HOST_ANY;
		address.port = 8999;

		server = enet_host_create(&address, 32, 2, 0, 0);

		if (server == NULL)
		{
			Message("Server", "Unable to bind to localhost.");
		}

		if (enet_host_compress_with_range_coder(server) < 0)
			Message("Server", "Unable to add compression.");

		ENetEvent event;
		while (enet_host_service(server, &event, 10000) > 0)
		{
			switch (event.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
					Message("Server", "Client connected: %x:%u", event.peer->address.host, event.peer->address.port);
					break;

				case ENET_EVENT_TYPE_RECEIVE:
					Message("Server", "Recieved packet from %s:%u, length %u: %s"
								, event.peer->data
								, event.channelID
								, event.packet->dataLength
								, event.packet->data);
					enet_packet_destroy(event.packet);
					break;

				case ENET_EVENT_TYPE_DISCONNECT:
					Message("Server", "Client %s disconnected.", event.peer->data);
					event.peer->data = NULL;
					break;
			}
		}
	 }
	 else /* CLIENT */
	 {
	 	ENetAddress address;
		ENetEvent event;
		ENetPeer *peer;
		ENetHost *client;

		enet_address_set_host(&address, "localhost");
		address.port = 8999;
		client = enet_host_create(NULL, 32, 2, 0, 0);

		if ((peer = enet_host_connect(client, &address, 2, 0)) == NULL)
		{
			Message("Client", "No available peers for initiating ENet connection.");
			exit(1);
		}

		if (enet_host_compress_with_range_coder(client) < 0)
			Message("Client", "Unable to add compression.");

		if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
		{
			Message("Client", "Connected.");

			ENetPacket *packet = enet_packet_create("packet", strlen("packet")+1, ENET_PACKET_FLAG_RELIABLE);

/*			enet_packet_resize(packet, strlen("packetfoo") + 1);
			strcpy(&packet->data[strlen("packet")], "foo");*/

			enet_peer_send(peer, 1, packet);

			enet_host_flush(client);;
		}
		else
			enet_peer_reset(peer);
	 }
    
	 /*
    while(win->IsOpen() && !inp->IsKeyDown(Input::ESC))
    {
		inp->Update();

		int x, y;
		inp->GetMousePos(&x, &y);
		Message("Main", "Mouse at %dx%d", x, y);

        win->Swap();
    }
	 */

	 enet_deinitialize();
/*
    res->Release(img);
    delete kernel;
	 */
    return 0;
}
