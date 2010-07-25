#include <Pxf/SampleModule/SampleModule.h>
#include <stdio.h>

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>

int main()
{
    Pxf::Kernel* kernel = Pxf::Kernel::GetInstance();
    Pxf::Message("Main", "Using kernel %x", kernel);

    kernel->DumpAvailableModules();

    return 0;
}