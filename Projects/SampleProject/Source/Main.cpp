#include <SampleModule.h>
#include <stdio.h>
#include <Pxf/Base/SharedLibrary.h>

int main()
{   
    Pxf::Base::SharedLibrary lib;
    
    if(lib.Load("SampleModule_ds.dll"))
        printf("loaded dll.\n");
    typedef int(*lulz)(int);
    printf("%x\n", lib.LookupName("lulz"));
    lulz f = (lulz)lib.LookupName("lulz");
    printf("%d\n", f(1));
    
    return 0;
    //return lulz();
}