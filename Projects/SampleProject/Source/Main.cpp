#include <Pxf/SampleModule/SampleModule.h>
#include <stdio.h>

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>

int main()
{
    Pxf::Kernel* kernel = Pxf::Kernel::GetInstance();
    Pxf::Message("Main", "Using kernel %x", kernel);
/*
#ifdef CONF_MODULAR
    // TODO: Module type does not makes sense anymore, since a module can contain multiple systems.
    kernel->RegisterModule(Pxf::Kernel::MODULE_TYPE_GRAPHICS, "SampleModule_ddyn.dll");
    
    // Linux/BSD
    //kernel->RegisterModule(Pxf::Kernel::MODULE_TYPE_GRAPHICS, "./SampleModule_ddyn.so");
    
    // MacOSX
    //kernel->RegisterModule(Pxf::Kernel::MODULE_TYPE_GRAPHICS, "./SampleModule_ddyn.dylib");
#else
    Pxf::Modules::SampleModule* m = new Pxf::Modules::SampleModule(0, 0);
    m->RegisterSystems(kernel);
#endif
*/
    

    return 0;
}