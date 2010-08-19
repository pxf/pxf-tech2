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
#include <Pxf/Resource/ResourceLoader.h>
#include <Pxf/Resource/Image.h>
#include <Pxf/Resource/Blob.h>

#include <ctime>

using namespace Pxf;


int main()
{
	Pxf::RandSetSeed(time(NULL));
	auto kernel = Pxf::Kernel::GetInstance();

    kernel->RegisterModule("pri", true);
	// RegisterSystem(const char* ao, unsigned filter) <<---- sätta filter på register module?
	kernel->RegisterSystem("pri", Pxf::System::SYSTEM_TYPE_GRAPHICS);
    kernel->RegisterModule("img", true);
    kernel->RegisterSystem("img", Pxf::System::SYSTEM_TYPE_RESOURCE_LOADER);
    kernel->DumpAvailableModules();

    auto gfx = kernel->GetGraphicsDevice();
	auto snd = kernel->GetAudioDevice();
    auto res = kernel->GetResourceManager();
    auto img = res->Acquire<Resource::Image>("test.png", 0);

	snd->Play(2);

	auto loader = res->FindResourceLoader<Resource::BlobLoader>(".blob");
	auto blob = loader->CreateFrom("aoeu", 5);



	/*
	// Resource::BinaryFile to complement? Resource::Blob, binary large object
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
    Graphics::Window* win = gfx->OpenWindow(&spec);
    
    while(win->IsOpen())
    {
        win->Swap();
    }
    */

	res->Release(img);
    delete kernel;
    return 0;
}