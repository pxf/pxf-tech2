#include <stdio.h>

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

#include <Pxf/Audio/AudioDevice.h>
#include <Pxf/Input/InputDevice.h>
#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Graphics/Window.h>
#include <Pxf/Graphics/WindowSpecifications.h>
#include <Pxf/Graphics/RenderBuffer.h>

#include <Pxf/Base/Hash.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Memory.h>
#include <Pxf/Base/Random.h>

#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/ResourceLoader.h>
#include <Pxf/Resource/Image.h>
#include <Pxf/Resource/Blob.h>
#include <Pxf/Resource/Sound.h>

#include <ctime>

#include <json/json.h>

using namespace Pxf;

int main()
{
	
	/* TODO: Make a resource loader for json */
	Json::Value root;
	Json::Reader reader;
	bool success = reader.parse("{\"honk\": 42}", root);
	if (success)
		Message("json", "%d", root.get("honk", 88).asInt());

    Pxf::RandSetSeed(time(NULL));
    Kernel* kernel = Pxf::Kernel::GetInstance();

	// Just load everything
    kernel->RegisterModule("pri", 0xFFFF, true);
    kernel->RegisterModule("img", 0xFFFF, true);
	kernel->RegisterModule("mesh", 0xFFFF, true);
	kernel->RegisterModule("snd", 0xFFFF, true);
    kernel->DumpAvailableModules();

    Graphics::GraphicsDevice* gfx = kernel->GetGraphicsDevice();
    Audio::AudioDevice* snd = kernel->GetAudioDevice();
    Input::InputDevice* inp = kernel->GetInputDevice();
    Resource::ResourceManager* res = kernel->GetResourceManager();
    res->DumpResourceLoaders();

    Resource::BlobLoader* loader = res->FindResourceLoader<Resource::BlobLoader>("blob");
    Resource::Blob* blob = loader->CreateFrom("aoeu", 5);

	Resource::Sound* sndclip = res->Acquire<Resource::Sound>("data/tick.ogg");
	int tick_id = snd->RegisterSound(sndclip);


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
    
	while(win->IsOpen())
    {
		inp->Update();
		if (inp->GetLastKey() == Input::ESC)
			break;

		char title[512];
		Format(title, "Renderer (fps: %d)", win->GetFPS());
		win->SetTitle(title);

		if (inp->GetLastButton() == Input::MOUSE_LEFT)
			snd->Play(tick_id);

        win->Swap();
		inp->ClearLastKey();
		inp->ClearLastButton();
    }
    

    delete kernel;
    return 0;
}

