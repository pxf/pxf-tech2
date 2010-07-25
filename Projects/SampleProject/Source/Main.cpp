#include <Pxf/SampleModule/SampleModule.h>
#include <stdio.h>

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>

int main()
{
    Pxf::Kernel* kernel = Pxf::Kernel::GetInstance();
    Pxf::Message("Main", "Using kernel %x", kernel);

    // kernel->Initialize(Pxf::Kernel::SYSTEM_TYPE_GRAPHICS, 'OGL2');
    kernel->RegisterModule("SampleModule_ddyn.dll", true);
    kernel->DumpAvailableModules();
    
    delete kernel;

    return 0;
}