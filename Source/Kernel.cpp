#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/SharedLibrary.h>
#include <Pxf/Base/Module.h>



Pxf::Kernel* Pxf::Kernel::s_Kernel = 0;

Pxf::Kernel::Kernel()
{
}

Pxf::Kernel::~Kernel()
{
}

bool Pxf::Kernel::RegisterModule(const char* _FilePath)
{
    Pxf::Base::SharedLibrary lib;
    if(lib.Load(_FilePath))
        Message("Kernel::RegisterModule","Loading '%s'", _FilePath);
    else
    {
        Message("Kernel::RegisterModule", "File not found: '%s'", _FilePath);
        return false;
    }
    
    typedef Pxf::Base::Module*(*CreateInstance_fun)(void);
    typedef void(*DestroyInstance_fun)(Pxf::Base::Module*);
        
    CreateInstance_fun CreateInstance = (CreateInstance_fun)lib.LookupName("CreateInstance");
    DestroyInstance_fun DestroyInstance = (DestroyInstance_fun)lib.LookupName("DestroyInstance");
    
    // Create a module instance to register the subsystems...
    // remove Module and replace with a few functions and a RegisterModules(Kernel*)...???
    Pxf::Base::Module* module = CreateInstance();
    Pxf::Message("Kernel::RegisterModule", "kernel_version = %d, module_ptr = %x", module->GetKernelVersion(), module);
    Pxf::Message("Kernel::RegisterModule", "module_version = %d, module_ptr = %x", module->GetApiVersion(), module);
    module->RegisterSystems(this);
    
    // All system required are registered, safe to delete the module.
    DestroyInstance(module);

    return true;
}

bool Pxf::Kernel::RegisterModule(Pxf::Base::Module* _Module)
{
    Message("Kernel::RegisterModule(Module*)", "Registered %s", _Module->GetIdentifier());
    return true;
}

void Pxf::Kernel::SetPreferredModule(ModuleType _ModuleType, const char* _ModuleID)
{

}