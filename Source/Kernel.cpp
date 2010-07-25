#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/SharedLibrary.h>
#include <Pxf/Module.h>



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
    
    typedef Pxf::Module*(*CreateInstance_fun)(void);
        
    CreateInstance_fun CreateInstance = (CreateInstance_fun)lib.LookupName("CreateInstance");
    DestroyModuleInstance_fun DestroyInstance = (DestroyModuleInstance_fun)lib.LookupName("DestroyInstance");
    
    // Create a module instance to register the subsystems...
    // remove Module and replace with a few functions and a RegisterModules(Kernel*)...???
    Pxf::Module* module = CreateInstance();
    Pxf::Message("Kernel::RegisterModule", "kernel_version = %d, module_ptr = %x", module->GetKernelVersion(), module);
    Pxf::Message("Kernel::RegisterModule", "module_version = %d, module_ptr = %x", module->GetApiVersion(), module);
    
    
    m_AvailableModules.push_back(new ModuleEntry_t(module, DestroyInstance));

    return true;
}

static void DestroyInstance(Pxf::Module* _Module)
{
    if (_Module)
        delete _Module;
}

bool Pxf::Kernel::RegisterModule(Pxf::Module* _Module)
{
    Message("Kernel::RegisterModule(Module*)", "Registered %s to kernel %x", _Module->GetIdentifier(), this);
    m_AvailableModules.push_back(new ModuleEntry_t(_Module, DestroyInstance));
    return true;
}

void Pxf::Kernel::SetPreferredModule(ModuleType _ModuleType, const char* _ModuleID)
{

}

void Pxf::Kernel::DumpAvailableModules()
{
    for(int i = 0; i < m_AvailableModules.size(); i++)
    {
        Message("Kernel::DumpAvailableModules", "%d. %s", i, m_AvailableModules[i]->module->GetIdentifier());
    }
}