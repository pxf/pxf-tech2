//#include <Pxf/SampleModule/SampleModule.h>
#include <stdio.h>

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

#include <Pxf/Audio/NullAudioDevice.h>

#include <Pxf/Graphics/Window.h>
#include <Pxf/Graphics/WindowSpecifications.h>

#include <Pxf/Modules/pri/DeviceGL2.h>

using namespace Pxf;

int main()
{
    Pxf::Kernel* kernel = Pxf::Kernel::GetInstance();
    Pxf::Message("Main", "Using kernel %x", kernel);

    // kernel->Initialize(Pxf::Kernel::SYSTEM_TYPE_GRAPHICS, 'OGL2');
    kernel->RegisterModule("SampleModule", false);
    kernel->RegisterModule("pri", true);
    kernel->DumpAvailableModules();
    
    //kernel->RegisterGraphicsDevice(new Pxf::Graphics::DeviceGL2(kernel));
    kernel->RegisterSystem("PortableRendererInput", Pxf::System::SYSTEM_TYPE_GRAPHICS);
    
    Pxf::Audio::AudioDevice* audio = kernel->GetAudioDevice();
    audio->Play(2);
    
    Pxf::Graphics::GraphicsDevice* video = kernel->GetGraphicsDevice();
    
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
    Graphics::Window* win = video->OpenWindow(&spec);
    
    while(win->IsOpen())
    {
        win->Swap();
    }
    
    delete kernel;

    return 0;
}