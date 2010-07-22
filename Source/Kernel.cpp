#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/SharedLibrary.h>
#include <Pxf/Base/Module.h>

Pxf::Kernel* Pxf::Kernel::s_Kernel = 0;

bool Pxf::Kernel::RegisterModule(ModuleType _ModuleType, const char* _FilePath)
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
    
    /*
    switch(GetModuleType())
    {
    case Pxf::Kernel::MODULE_TYPE_GRAPHICS: system = Pxf::System::Graphics* gfx = (Pxf::System::Graphics*)CreateInstance();
                                            break
    }
    */
    
    Pxf::Base::Module* module = CreateInstance();
    Pxf::Message("Kernel::RegisterModule", "kernel_version = %d, module_ptr = %x", module->GetKernelVersion(), module);
    Pxf::Message("Kernel::RegisterModule", "module_version = %d, module_ptr = %x", module->GetApiVersion(), module);
    module->RegisterSystems(this);
    //DestroyInstance(module);
    
    
    return true;
}

void Pxf::Kernel::SetPreferredModule(ModuleType _ModuleType, const char* _ModuleID)
{

}