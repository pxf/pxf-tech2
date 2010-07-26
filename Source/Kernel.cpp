#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/SharedLibrary.h>
#include <Pxf/Module.h>
#include <Pxf/Base/Utils.h>

Pxf::Kernel* Pxf::Kernel::s_Kernel = 0;
const unsigned Pxf::Kernel::KERNEL_VERSION = PXF_PACKSHORT2(1, 1);

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

    Pxf::SharedLibrary* lib = new Pxf::SharedLibrary();
    
    if(!lib->Load(_FilePath))
    {
        Message("Kernel", "File not found: '%s'", _FilePath);
        return false;
    }
    
    typedef Pxf::Module*(*CreateInstance_fun)(void);
        
    CreateInstance_fun CreateInstance = (CreateInstance_fun)lib->LookupName("CreateInstance");
    DestroyModuleInstance_fun DestroyInstance = (DestroyModuleInstance_fun)lib->LookupName("DestroyInstance");
    
    if(!CreateInstance)
    {
        Pxf::Message("Kernel", "CreateInstance: %x", CreateInstance);
        //delete lib;
        return false;
    }
    
    Pxf::Module* module = CreateInstance();
    Pxf::Message("Kernel", "Loaded '%s' (0x%x)", _FilePath, module);
    
    // Check that the module isn't already available, or override
    bool replaced = false;
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
                Message("Kernel", "'%s' is overriding built-in '%s'.", _FilePath, module->GetIdentifier());
                
                // Remove built-in module
                m_AvailableModules[i]->destroy(m_AvailableModules[i]->module);
                if (m_AvailableModules[i]->dynlib)
                    delete m_AvailableModules[i]->dynlib;
                    
                // And replace with the newly loaded one.
                m_AvailableModules[i]->module = module;
                m_AvailableModules[i]->dynlib = lib;
                m_AvailableModules[i]->destroy = DestroyInstance;
                
                replaced = true;
                break;
            }
        }
    }
    
    unsigned short kmaj, kmin;
    unsigned short mmaj, mmin;
    Pxf::UnpackShort2(module->GetKernelVersion(), &kmaj, &kmin);
    Pxf::UnpackShort2(module->GetApiVersion(), &mmaj, &mmin);
    unsigned short currkmaj, currkmin;
    Pxf::UnpackShort2(Pxf::Kernel::KERNEL_VERSION, &currkmaj, &currkmin);
    if (kmaj < currkmaj || (kmaj == currkmaj && kmin < currkmin))
    {
        Message("Kernel", "Warning, kernel version mismatch (%d.%d is recommended)", currkmaj, currkmin);
    }
    
    unsigned short currmmaj, currmmin;
    Pxf::UnpackShort2(Pxf::Module::MODULE_VERSION, &currmmaj, &currmmin);
    if (mmaj < currmmaj || (mmaj == currmmaj && mmin < currmmin))
    {
        Message("Kernel", "Warning - Module API version mismatch (%d.%d is recommended)", currmmaj, currmmin);
    }
    
    Message("Kernel", "Registered %s (dylib, kv: %d.%d, mv: %d.%d) to kernel %x", module->GetIdentifier(), kmaj, kmin, mmaj, mmin, this);
    
    if (!replaced)
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

bool Pxf::Kernel::RegisterSystem(const char* _ModuleID, SystemType _SystemType)
{
    for(int i = 0; i < m_AvailableModules.size(); i++)
    {
        Pxf::Module* mod = m_AvailableModules[i]->module;
        if (strcmp(mod->GetIdentifier(), _ModuleID) == 0)
        {
            mod->RegisterSystem(this, _SystemType);
            return true;
            break;
        }
    }
    return false;
}

void Pxf::Kernel::DumpAvailableModules()
{
    Message("Kernel", "Dumping available modules:");
    for(int i = 0; i < m_AvailableModules.size(); i++)
    {
		const char* path = "built-in";
		if (m_AvailableModules[i]->dynlib)
			path = m_AvailableModules[i]->dynlib->GetFilePath();
        Message("Kernel", "%d. \t%s (%s)", i, m_AvailableModules[i]->module->GetIdentifier(), path);
    }
}