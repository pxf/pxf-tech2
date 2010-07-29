//#include <Pxf/SampleModule/SampleModule.h>
#include <stdio.h>

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

#include <Pxf/AudioDevice/NullAudioDevice.h>

int main()
{
    Pxf::Kernel* kernel = Pxf::Kernel::GetInstance();
    Pxf::Message("Main", "Using kernel %x", kernel);

    // kernel->Initialize(Pxf::Kernel::SYSTEM_TYPE_GRAPHICS, 'OGL2');
    kernel->RegisterModule("SampleModule", false);
    kernel->RegisterModule("pri", true);
    kernel->DumpAvailableModules();
    
    kernel->RegisterAudioDevice(new Pxf::Audio::NullAudioDevice(kernel));
    Pxf::Audio::AudioDevice* audio = kernel->GetAudioDevice();
    
    audio->Play(2);
    
    if (kernel->RegisterSystem("SampleModule", Pxf::System::SYSTEM_TYPE_GRAPHICS))
    {

    }
    
    if (kernel->RegisterSystem("PortableRendererInput", Pxf::System::SYSTEM_TYPE_GRAPHICS, 'GL2'))
    {

    }
    
    delete kernel;

    return 0;
}