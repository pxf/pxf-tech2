//#include <Pxf/SampleModule/SampleModule.h>
#include <stdio.h>

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

int main()
{
    Pxf::Kernel* kernel = Pxf::Kernel::GetInstance();
    Pxf::Message("Main", "Using kernel %x", kernel);

    // kernel->Initialize(Pxf::Kernel::SYSTEM_TYPE_GRAPHICS, 'OGL2');
    kernel->RegisterModule("SampleModule", false);
    kernel->RegisterModule("pri", true);
    kernel->DumpAvailableModules();
    
    if (kernel->RegisterSystem("SampleModule", Pxf::Kernel::SYSTEM_TYPE_GRAPHICS))
    {

    }
    
    if (kernel->RegisterSystem("PortableRendererInput", Pxf::Kernel::SYSTEM_TYPE_GRAPHICS, 'GL2'))
    {

    }
    
    delete kernel;

    return 0;
}