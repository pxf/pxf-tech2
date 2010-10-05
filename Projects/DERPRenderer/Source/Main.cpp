#include <stdio.h>

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Timer.h>
#include <Pxf/Base/Logger.h>
#include <RemoteLogWriter.h>

#include <Pxf/Audio/AudioDevice.h>
#include <Pxf/Input/InputDevice.h>
#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Graphics/Window.h>
#include <Pxf/Graphics/WindowSpecifications.h>
#include <Pxf/Graphics/RenderBuffer.h>
#include <Pxf/Graphics/Model.h>

#include <Pxf/Graphics/VertexBuffer.h>
#include <Pxf/Graphics/VertexBufferDefs.h>

#include <Pxf/Base/Hash.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Memory.h>
#include <Pxf/Base/Random.h>

#include <Pxf/Modules/pri/OpenGL.h>

#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/ResourceLoader.h>
#include <Pxf/Resource/Image.h>
#include <Pxf/Resource/Json.h>
#include <Pxf/Resource/Sound.h>
#include <Pxf/Resource/Font.h>
#include <Pxf/Resource/Text.h>
#include <Pxf/Resource/Mesh.h>

#include <Pxf/Network/NetworkDevice.h>

#include <Pxf/Modules/pri/RenderBufferGL2.h>

#include "Renderer.h"
#include "SimpleQuad.h"

#include <ctime>

#include "Camera.h"

using namespace Pxf;
using namespace Math;
using namespace Graphics;
using namespace Derp;


int main()
{
	Pxf::RandSetSeed(time(NULL));
	Kernel* kernel = Pxf::Kernel::GetInstance();

	unsigned main_tag = kernel->CreateTag("Main");

	kernel->Log(main_tag | Logger::IS_CRITICAL, "Honk %s", "Tonk");
	kernel->Log(main_tag | Logger::IS_DEBUG, "Honk %s", "Tonk LOL");

	// Just load everything
	kernel->RegisterModule("pri", 0xFFFF, true);
	kernel->RegisterModule("img", 0xFFFF, true);
	kernel->RegisterModule("mesh", 0xFFFF, true);
	kernel->RegisterModule("snd", 0xFFFF, true);
	kernel->RegisterModule("json", 0xFFFF, true);
	kernel->RegisterModule("net", 0xFFFF, true);
	kernel->DumpAvailableModules();

	Graphics::GraphicsDevice* gfx = kernel->GetGraphicsDevice();
	Audio::AudioDevice* snd = kernel->GetAudioDevice();
	Input::InputDevice* inp = kernel->GetInputDevice();
	Resource::ResourceManager* res = kernel->GetResourceManager();
	Network::NetworkDevice* net = kernel->GetNetworkDevice();
	res->DumpResourceLoaders();

	// load settings
	Resource::Json* jdoc = res->Acquire<Resource::Json>("data/config.json");
	Json::Value settings;
	if (jdoc)
		settings = jdoc->GetRoot();
	else
	{
		settings["audio"]["buffersize"] = 1024;
		settings["audio"]["max_voices"] = 8;
		settings["input"]["mouse_accel"] = 0.9f;
		settings["video"]["width"] = 800;
		settings["video"]["height"] = 600;
		settings["video"]["vsync"] = false;

		Resource::JsonLoader* jld = res->FindResourceLoader<Resource::JsonLoader>("json");
		if (jld)
		{
			jdoc = jld->CreateEmpty();
			jdoc->SetRoot(settings);
			jdoc->SaveToDisk("data/config.json");
			kernel->Log(main_tag | Logger::IS_INFORMATION, "Saving new config!");
		}
	}

	snd->Initialize(settings["audio"].get("buffersize", 512).asUInt()
				   ,settings["audio"].get("max_voices", 8).asUInt());


	unsigned netlogtag = net->AddTag("log");
	Network::Server* server = net->CreateServer();
	server->Bind(9001);
	kernel->RegisterLogger(new RemoteLogWriter(server, netlogtag));

	Graphics::WindowSpecifications spec;
	spec.Width = 512;//renderer->m_Width;//settings["video"].get("width", 800).asInt();
	spec.Height = 512;//renderer->m_Height;//settings["video"].get("height", 600).asInt();
	spec.ColorBits = 24;
	spec.AlphaBits = 8;
	spec.DepthBits = 8;
	spec.StencilBits = 0;
	spec.FSAASamples = 0;
	spec.Fullscreen = false;
	spec.Resizeable = false;
	spec.VerticalSync = settings["video"].get("vsync", true).asBool();
	Graphics::Window* win = gfx->OpenWindow(&spec);

	Resource::Mesh* aoeu = res->Acquire<Resource::Mesh>("data/test.ctm");
	Graphics::Model* m = gfx->CreateModel(aoeu);
	
	// Setup renderer
	Derp::Renderer* renderer = new Derp::Renderer(settings["network"].get("port", 7005).asUInt());
	
	// Setup full screen quad
	SimpleQuad* finalquad = new SimpleQuad(0.0f, 0.0f, 1.0f, 1.0f);

	Timer t;
	while(win->IsOpen())
	{
		t.Start();	

		// Execute renderer/pipeline
		renderer->Execute();
		
		// Setup ogl
		gfx->SetViewport(0, 0, win->GetWidth(), win->GetHeight());
	  //Math::Mat4 prjmat = Math::Mat4::Ortho(0, win->GetWidth(), win->GetHeight(), 0, 1.0f, 10000.0f);
		Math::Mat4 prjmat = Math::Mat4::Ortho(0, 1.0f, 1.0f, 0, -1.0f, 10000.0f);
		gfx->SetProjection(&prjmat);
		glClearColor(26.0f/255.0f,26.0f/255.0f,26.0f/255.0f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Render final output
		glEnable(GL_TEXTURE_2D);
		gfx->BindTexture(renderer->GetResult());
		finalquad->Draw();
		
		inp->Update();
		
		if (inp->GetLastKey() == Input::ESC)
			break;

		if (inp->GetLastKey() == Input::F5)
		{
			win->Swap();
			//Resource::Image* img = gfx->CreateImageFromTexture(renderer->GetResult());
			Texture* tex = gfx->CreateTextureFromFramebuffer();
			Resource::Image* img = gfx->CreateImageFromTexture(tex);

			gfx->DestroyTexture(tex);
			img->SaveAs("data/screenshot.tga");
			delete img;
		}

/*		if (inp->GetLastKey() == Input::F6)
		{
			win->Swap();
			Texture* tex = gfx->CreateTextureFromFramebuffer();
			Resource::Image* img = gfx->CreateImageFromTexture(tex);

			// TODO: Also send some identifier for the last texture.
			int height = img->Height();
			server->SendAllL(packet_result, (const char*)&height, 1);
			server->SendAllL(packet_result, (const char*)img->Ptr(), img->Height()*img->Width()*img->Channels());

			gfx->DestroyTexture(tex);
			delete img;
		}
		*/
		
		inp->ClearLastKey();
		inp->ClearLastButton();
		
		char title[512];
		t.Stop();
		Format(title, "Renderer (fps: %d, processing time: %d ms)", win->GetFPS(), t.Interval());
		win->SetTitle(title);

		win->Swap();

		/*Network::Packet* packet = server->RecvNonBlocking(0);
		if (packet != NULL)
			if (packet->GetTag() == packet_renderer)
			{
				kernel->Log(main_tag | Logger::IS_DEBUG, "aoeu, lol");
			}
			*/

		//glPopMatrix();
	}
	
	delete finalquad;
	delete renderer;

	delete kernel;
	return 0;
}

