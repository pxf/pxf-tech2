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
    kernel->RegisterModule("SampleModule_ddyn.dll", false);
    kernel->RegisterModule("pri_ddyn.dll", false);
    kernel->DumpAvailableModules();
    
    //if (kernel->RegisterSystem("SampleModule", Pxf::Kernel::SYSTEM_TYPE_GRAPHICS))
    {
        // SampleModule contained a graphics system, available via kernel->GetGraphicsSystem()?
    }
    
    delete kernel;

    return 0;
}