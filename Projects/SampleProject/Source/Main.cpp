//#include <Pxf/SampleModule/SampleModule.h>
#include <stdio.h>

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

#include <Pxf/Audio/AudioDevice.h>
#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Graphics/Window.h>
#include <Pxf/Graphics/WindowSpecifications.h>

#include <Pxf/Base/Hash.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Memory.h>
#include <Pxf/Base/Random.h>

#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/Image.h>

#include <ctime>

using namespace Pxf;

int main()
{
	Pxf::RandSetSeed(time(NULL));
    Pxf::Kernel* kernel = Pxf::Kernel::GetInstance();

    kernel->RegisterModule("pri", true);
	kernel->RegisterSystem("pri", Pxf::System::SYSTEM_TYPE_GRAPHICS);
    kernel->RegisterModule("img", true);
    kernel->RegisterSystem("img", Pxf::System::SYSTEM_TYPE_RESOURCE_LOADER);
    kernel->DumpAvailableModules();
    
    Pxf::Audio::AudioDevice* audio = kernel->GetAudioDevice();
    audio->Play(2);
    
    Pxf::Graphics::GraphicsDevice* video = kernel->GetGraphicsDevice();
    
    Pxf::Resource::ResourceManager* res = kernel->GetResourceManager();
    Resource::Image* img = res->Acquire<Resource::Image>("test.png", 0);
    
	/*
	// Resource::BinaryFile to complement?
	Resource::TextFile* shadersrc = res->Acquire<Resource::TextFile>("fiddle.glsl");
	Graphics::Shader* shader = video->CreateShader(Resource::TextFile* _source);
	shader->SetBlahBlah();
	*/

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
    /*
    Graphics::Window* win = video->OpenWindow(&spec);
    
    while(win->IsOpen())
    {
        win->Swap();
    }
    */

	res->Release(img);
    delete kernel;
    return 0;
}