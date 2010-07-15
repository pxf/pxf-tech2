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
        Message("Kernel::RegisterModule", "File not found: '%s'.", _FilePath);
        return false;
    }
        
    typedef unsigned(*GetEngineVersion_fun)(void);
    typedef unsigned(*GetModuleVersion_fun)(void);
    typedef unsigned(*GetModuleType_fun)(void);
    typedef Pxf::Base::Module*(*CreateInstance_fun)(void);
    typedef void(*DestroyInstance_fun)(Pxf::Base::Module*);
        
    GetEngineVersion_fun GetEngineVersion = (GetEngineVersion_fun)lib.LookupName("GetEngineVersion");
    GetModuleVersion_fun GetModuleVersion = (GetModuleVersion_fun)lib.LookupName("GetModuleVersion");
    GetModuleType_fun GetModuleType = (GetModuleType_fun)lib.LookupName("GetModuleType");
    CreateInstance_fun CreateInstance = (CreateInstance_fun)lib.LookupName("CreateInstance");
    DestroyInstance_fun DestroyInstance = (DestroyInstance_fun)lib.LookupName("DestroyInstance");
        
    Pxf::Message("Kernel::RegisterModule", "EngineVersion = %d", GetEngineVersion());
    Pxf::Message("Kernel::RegisterModule", "ModuleVersion = %d", GetModuleVersion());
    Pxf::Message("Kernel::RegisterModule", "ModuleType = %d", GetModuleType());
    
    Pxf::Base::Module* module = CreateInstance();
    Pxf::Message("Kernel::RegisterModule", "module_version = %d, module_ptr = %x", module->GetApiVersion(), module);
    DestroyInstance(module);
    
    
    return true;
}

void Pxf::Kernel::SetPreferredModule(ModuleType _ModuleType, const char* _ModuleID)
{

}