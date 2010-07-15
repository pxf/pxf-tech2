#include <Pxf/SampleModule/SampleModule.h>
#include <stdio.h>

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>

int main()
{
    Pxf::Kernel* kernel = Pxf::Kernel::GetInstance();
    Pxf::Message("Main", "Using kernel %x", kernel);
    kernel->RegisterModule(Pxf::Kernel::MODULE_TYPE_GRAPHICS, "SampleModule_ds.dll");
    

    return 0;
}