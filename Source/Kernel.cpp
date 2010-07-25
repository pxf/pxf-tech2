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
    for(int i = 0; i < m_AvailableModules.size(); i++)
    {
        Pxf::Module* m = m_AvailableModules[i]->module;
        m_AvailableModules[i]->destroy(m);
        if (m_AvailableModules[i]->dynlib)
            delete m_AvailableModules[i]->dynlib;
        delete m_AvailableModules[i];
    }
}

bool Pxf::Kernel::RegisterModule(const char* _FilePath, bool _OverrideBuiltin)
{
    // TODO:
    // Check if _FilePath ends with .dll, .so or .dylib
    // if not, append the correct one depending on platform

    Pxf::Base::SharedLibrary* lib = new Pxf::Base::SharedLibrary();
    
    if(lib->Load(_FilePath))
        Message("Kernel","Loading '%s'", _FilePath);
    else
    {
        Message("Kernel", "File not found: '%s'", _FilePath);
        return false;
    }
    
    typedef Pxf::Module*(*CreateInstance_fun)(void);
        
    CreateInstance_fun CreateInstance = (CreateInstance_fun)lib->LookupName("CreateInstance");
    DestroyModuleInstance_fun DestroyInstance = (DestroyModuleInstance_fun)lib->LookupName("DestroyInstance");
    
    Pxf::Module* module = CreateInstance();
    Pxf::Message("Kernel", "kernel_version = %d, module_ptr = %x", module->GetKernelVersion(), module);
    Pxf::Message("Kernel", "module_version = %d, module_ptr = %x", module->GetApiVersion(), module);
    
    // Check that the module isn't already available, or override
    for(int i = 0; i < m_AvailableModules.size(); i++)
    {
        if (strcmp(m_AvailableModules[i]->module->GetIdentifier(), module->GetIdentifier()) == 0)
        {
            if (!_OverrideBuiltin)
            {
                Message("Kernel", "Module '%s' is already built-in.", module->GetIdentifier());
                DestroyInstance(module);
                delete lib;
                return false;
            }
            else
            {
                Message("Kernel", "Module '%x' is overriding built-in '%x'", module, m_AvailableModules[i]->module);
                
                // Remove built-in module
                m_AvailableModules[i]->destroy(m_AvailableModules[i]->module);
                if (m_AvailableModules[i]->dynlib)
                    delete m_AvailableModules[i]->dynlib;
                    
                // And replace with the newly loaded one.
                m_AvailableModules[i]->module = module;
                m_AvailableModules[i]->dynlib = lib;
                m_AvailableModules[i]->destroy = DestroyInstance;
                
                return true;
            }
        }
    }
    
    Message("Kernel", "Registered %s (sharedlib) to kernel %x", module->GetIdentifier(), this);
    m_AvailableModules.push_back(new ModuleEntry_t(lib, module, DestroyInstance));

    return true;
}

static void DestroyBuiltInInstance(Pxf::Module* _Module)
{
    if (_Module)
        delete _Module;
}

bool Pxf::Kernel::RegisterModule(Pxf::Module* _Module)
{
    m_AvailableModules.push_back(new ModuleEntry_t(_Module, DestroyBuiltInInstance));
    return true;
}

void Pxf::Kernel::SetPreferredModule(SystemType _SystemType, const char* _ModuleID)
{

}

void Pxf::Kernel::DumpAvailableModules()
{
    Message("Kernel", "Dumping available modules:");
    for(int i = 0; i < m_AvailableModules.size(); i++)
    {
        Message("Kernel", "%d. \t%s", i, m_AvailableModules[i]->module->GetIdentifier());
    }
}