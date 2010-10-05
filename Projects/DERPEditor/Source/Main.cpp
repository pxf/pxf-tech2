//#include <Pxf/SampleModule/SampleModule.h>
#include <stdio.h>

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Timer.h>

#include <Pxf/Audio/AudioDevice.h>
#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Graphics/Window.h>
#include <Pxf/Graphics/WindowSpecifications.h>
#include <Pxf/Graphics/RenderBuffer.h>
#include <Pxf/Graphics/FrameBufferObject.h>
#include <Pxf/Graphics/Shader.h>
#include <Pxf/Graphics/Model.h>

#include <Pxf/Base/Hash.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Memory.h>
#include <Pxf/Base/Random.h>

#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/ResourceLoader.h>
#include <Pxf/Resource/Image.h>
#include <Pxf/Resource/Blob.h>

#include <Pxf/Network/NetworkDevice.h>

#include <enet/enet.h>

#include <ctime>

#include "QuadBatch.h"
#include "TexturedQuadBatch.h"
#include "LuaApp.h"
#include <Pxf/Modules/pri/OpenGL.h>


using namespace Pxf;
using namespace DERPEditor;

int main()
{
    Pxf::RandSetSeed(time(NULL));
    Kernel* kernel = Pxf::Kernel::GetInstance();

	// Modules
    kernel->RegisterModule("pri", Pxf::System::SYSTEM_TYPE_GRAPHICSDEVICE | Pxf::System::SYSTEM_TYPE_INPUTDEVICE, true);
    kernel->RegisterModule("img", Pxf::System::SYSTEM_TYPE_RESOURCE_LOADER, true);
	kernel->RegisterModule("snd", 0xFFFF, true);
	kernel->RegisterModule("net", Pxf::System::SYSTEM_TYPE_NETWORKDEVICE, true);
    kernel->DumpAvailableModules();

	// Device contexts
    Graphics::GraphicsDevice*	gfx = kernel->GetGraphicsDevice();
    Input::InputDevice*			inp = kernel->GetInputDevice();
    Resource::ResourceManager*	res = kernel->GetResourceManager();
	Network::NetworkDevice*     net = kernel->GetNetworkDevice();

    res->DumpResourceLoaders();

	// Window specifications
    Graphics::WindowSpecifications spec;
    spec.Width = 1024;
    spec.Height = 600;
    spec.ColorBits = 24;
    spec.AlphaBits = 8;
    spec.DepthBits = 24;
    spec.StencilBits = 1;
    spec.FSAASamples = 0;
    spec.Fullscreen = false;
    spec.Resizeable = false;
    spec.VerticalSync = true;
    
    Graphics::Window* win = gfx->OpenWindow(&spec);
    
    // Setup lua application
    LuaApp* app = new LuaApp(win, "data/editor.lua");
    app->Boot();
    bool running = true;
	
	glfwDisable(GLFW_AUTO_POLL_EVENTS);
	glfwDisable(GLFW_MOUSE_CURSOR);
	
	Pxf::Timer racetimer;
	uint64 framelength = 16;
	uint64 frametotal = 0;
	
	racetimer.Start();

	// bind texturing Note: can we move this to the graphics device? 
	glEnable(GL_TEXTURE_2D);

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

        app->Draw();
    }
    
    delete app;
    delete kernel;
    return 0;
}

