#include <Pxf/SampleModule/SampleModule.h>
#include <stdio.h>
#include <Pxf/Base/SharedLibrary.h>
#include <Pxf/Kernel.h>

int main()
{   
    Pxf::Base::SharedLibrary lib;
    
    Pxf::KernelLocator kernel_locator;
    Pxf::Kernel* kernel = kernel_locator.GetInstance();
    
    if(lib.Load("SampleModule_ds.dll"))
        printf("loaded dll.\n");
    typedef int(*lulz)(int);
    printf("%x\n", lib.LookupName("lulz"));
    lulz f = (lulz)lib.LookupName("lulz");
    printf("%d\n", f(1));
    
    return 0;
}