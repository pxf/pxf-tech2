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
    Pxf::Kernel* kernel = Pxf::Kernel::GetInstance();
    Pxf::Message("Main", "Using kernel %x, %d", kernel, Pxf::HashPtr(kernel));
    Pxf::Message("Main", "Test: %d", Pxf::Hash("Heyo", 4));
    
    const char* str = "HEy mang!";
    if (!Pxf::IsPrefix(str, "Hey") && Pxf::IsPrefixI(str, "HEY"))
        Pxf::Message("Main", "Heyo!");
        
    Pxf::RandSetSeed(time(NULL));
    Message("Main", "%u %d %f %f", Pxf::RandUI32(), Pxf::RandI32()
                                 , Pxf::RandFP64(), Pxf::RandFP32());
    
    kernel->RegisterModule("pri", true);
    kernel->RegisterModule("img", true); // comment out to make it work
    kernel->RegisterSystem("PortableRendererInput", Pxf::System::SYSTEM_TYPE_GRAPHICS);
    kernel->RegisterSystem("GenericImageImporter", Pxf::System::SYSTEM_TYPE_RESOURCE_LOADER);
    kernel->DumpAvailableModules();
    
    Pxf::Audio::AudioDevice* audio = kernel->GetAudioDevice();
    audio->Play(2);
    
    Pxf::Graphics::GraphicsDevice* video = kernel->GetGraphicsDevice();
    
    
    Pxf::Resource::ResourceManager* res = kernel->GetResourceManager();
    //Resource::Image* img = res->Acquire<Resource::Image>("test.png", 0);
    //res->Release(img);
    
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
    delete kernel;

    return 0;
}