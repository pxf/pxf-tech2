#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/SharedLibrary.h>

#include <cstdio>

Pxf::Kernel* Pxf::Kernel::s_Kernel = 0;

bool Pxf::Kernel::RegisterModule(ModuleType _ModuleType, const char* _FilePath)
{
     Pxf::Base::SharedLibrary lib;
    if(lib.Load(_FilePath))
        printf("loaded dll.\n");
    typedef int(*lulz)(int);
    printf("%x\n", lib.LookupName("lulz"));
    lulz f = (lulz)lib.LookupName("lulz");
    printf("%d\n", f(1));
    return false;
}

void Pxf::Kernel::SetPreferredModule(ModuleType _ModuleType, const char* _ModuleID)
{

}