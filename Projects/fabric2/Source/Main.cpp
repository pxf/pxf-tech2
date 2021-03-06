#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Timer.h>
#include <Pxf/Base/Logger.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Random.h>
#include <Pxf/Base/Memory.h>
#include <Pxf/Base/Platform.h>
#include <Pxf/Math/Math.h>
#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Graphics/Window.h>
#include <Pxf/Graphics/WindowSpecifications.h>
#include <Pxf/Graphics/Texture.h>
#include <Pxf/Graphics/PrimitiveBatch.h>
#include <Pxf/Graphics/Model.h>
#include <Pxf/Resource/Text.h>

#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/ResourceLoader.h>

#include <Pxf/Input/InputDevice.h>
#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Modules/pri/OpenGL.h>

#include <ctime>
#include <cstdio>
#include <cstdlib>
#include "App.h"
#include "Grid.h"
#include "../PreloadedResources.h"

#include <Pxf/Base/Stream.h>

using namespace Pxf;
using namespace Graphics;
using namespace Math;



int main(int argc, char* argv[])
{
	Pxf::RandSetSeed(time(NULL));
	Kernel* kernel = Pxf::Kernel::GetInstance();
	kernel->RegisterModule("pri", Pxf::System::SYSTEM_TYPE_GRAPHICSDEVICE |
								  Pxf::System::SYSTEM_TYPE_INPUTDEVICE, true);
	kernel->RegisterModule("snd", Pxf::System::SYSTEM_TYPE_RESOURCE_LOADER |
								  Pxf::System::SYSTEM_TYPE_AUDIODEVICE, true);
	kernel->RegisterModule("json",Pxf::System::SYSTEM_TYPE_RESOURCE_LOADER, true);
	kernel->RegisterModule("img", Pxf::System::SYSTEM_TYPE_RESOURCE_LOADER, true);

	unsigned logtag = kernel->CreateTag("main");
	kernel->Log(logtag | Logger::IS_INFORMATION, "-----------------------------------------------------------");
	kernel->Log(logtag | Logger::IS_INFORMATION, "Any errors displayed above are safe to ignore.");
	kernel->Log(logtag | Logger::IS_INFORMATION, "These libraries are already embedded within the executable.");
	kernel->Log(logtag | Logger::IS_INFORMATION, "-----------------------------------------------------------");

	Resource::ResourceManager* res = kernel->GetResourceManager();
	Graphics::GraphicsDevice* gfx = kernel->GetGraphicsDevice();
	Input::InputDevice* inp = kernel->GetInputDevice();
	
	res->DumpResourceLoaders();

	// Add preloaded resources
	if (preloaded_files)
	{
		for(int i = 0; i < sizeof(preloaded_files)/sizeof(preloaded_files[0]); i++)
		{
			res->RegisterCachedFile(preloaded_files[i].path
				,preloaded_files[i].data
				,preloaded_files[i].size);
		}
	}

	Resource::Text* luamain = res->Acquire<Resource::Text>("jam/main.lua", "txt");

	Graphics::WindowSpecifications spec;
	spec.Width = 512;
	spec.Height = 512;
	spec.ColorBits = 24;
	spec.AlphaBits = 8;
	spec.DepthBits = 24;
	spec.StencilBits = 0;
	spec.FSAASamples = 0;
	spec.Fullscreen = false;
	spec.Resizeable = false;
	spec.VerticalSync = true;
	Graphics::Window* win = gfx->OpenWindow(&spec);

	if (!win)
	{
		kernel->Log(0, "No window. Exiting.");
		return 1;
	}
	
	// Start from different script than default.lua ?
	Util::String appscript = "jam/main.lua";
	if (argc > 1)
		appscript = argv[1];
	
	// Fabric setup
	Fabric::App* app = new Fabric::App(win, appscript.c_str());
	app->Boot();
	bool running = true;
	bool guihit = false;

	Pxf::Timer racetimer;
	uint64 framelength = 16;
	uint64 frametotal = 0;
	racetimer.Start();

	Grid<int> g(512,512,8);

	for(int i=0; i < 100; i++)
	{
		int x = rand() % 512;
		int y = rand() % 512;

		GridObject<int>* o = new GridObject<int>();
		o->r = 8;
		o->p = Pxf::Math::Vec2i(x,y);

		g.Insert(o);
	}

	while(win->IsOpen() && running)
	{
		while (frametotal < framelength)
		{
			racetimer.Stop();
			frametotal += racetimer.Interval();
			racetimer.Start();
			glfwSleep(0.004);
		}
		frametotal = 0;
		inp->Update();
			
		running = app->Update();
		guihit = app->GuiHit();
		
		app->Draw();
		inp->ClearLastKey();

		char tmp[4096];
		sprintf(tmp, "%d", win->GetFPS());
		win->SetTitle(tmp);

		//win->Swap();
	}
	
	delete app;
	delete kernel;

	return 0;
}
