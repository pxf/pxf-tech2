#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/SharedLibrary.h>
#include <Pxf/Module.h>
#include <Pxf/Base/Utils.h>

#include <Pxf/AudioDevice/AudioDevice.h>

#ifdef CONF_FAMILY_UNIX
#include <string.h> // strcmp
#endif

Pxf::Kernel* Pxf::Kernel::s_Kernel = 0;
const unsigned Pxf::Kernel::KERNEL_VERSION = PXF_PACKSHORT2(1, 1);

Pxf::Kernel::Kernel()
    : m_AudioDevice(0)
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

void Pxf::Kernel::RegisterAudioDevice(Pxf::AudioDevice* _Device)
{
    Pxf::Message("Kernel", "Registering audio device '%s'", _Device->GetIdentifier());
    m_AudioDevice = _Device;
}


//TODO: Should the build file define PXF_MODULE_EXT instead?
static const char* get_full_module_ext()
{
    #ifdef CONF_DEBUG
        #define MODULE_SUFFIX "_ddyn"
    #else
        #define MODULE_SUFFIX "_s"
    #endif
    
    #if defined(CONF_FAMILY_WINDOWS)
        #define MODULE_EXT ".dll"
    #elif defined(CONF_FAMILY_UNIX) && defined(CONF_PLATFORM_MACOSX)
        #define MODULE_EXT ".dylib"
    #else
        #define MODULE_EXT ".so"
    #endif
    #define RETVAL MODULE_SUFFIX MODULE_EXT
    return RETVAL;
}

static const char* get_module_ext()
{
    #if defined(CONF_FAMILY_WINDOWS)
        #define MODULE_EXT ".dll"
    #elif defined(CONF_FAMILY_UNIX) && defined(CONF_PLATFORM_MACOSX)
        #define MODULE_EXT ".dylib"
    #else
        #define MODULE_EXT ".so"
    #endif
    return MODULE_EXT;
}

static bool is_suffix(const char *s, const char *p)
{
	if (!s || !p) return 0;
	p += strlen(p) - 1;
	s += strlen(s) - 1;
	for(;*p && *s;s--,p--)
		if(*p != *s)
			return false;
	return true;
}

static bool is_prefix(const char *s, const char *p)
{
	if (!s || !p) return 0;
	for(;*p && *s;p++,s++)
		if(*p != *s)
			return false;
	return true;
}

bool Pxf::Kernel::RegisterModule(const char* _FilePath, bool _OverrideBuiltin)
{
    // If the file is missing extention, add one that's appropriate for the platform.
    const char* suffix = get_full_module_ext();
    char FilePath[256] = {0};
    unsigned len = strlen(_FilePath);
    unsigned offset = 0;
    
    // Also add prefixing ./ on unix
    #if defined(CONF_PLATFORM_MACOSX)
        if (!is_prefix(_FilePath, "./"))
        {
            offset = 5;
            FilePath[0] = '.';
            FilePath[1] = '/';
            FilePath[2] = 'l';
            FilePath[3] = 'i';
            FilePath[4] = 'b';
        }
    #elif defined(CONF_FAMILY_UNIX)
        if (!is_prefix(_FilePath, "./"))
        {
            offset = 2;
            FilePath[0] = '.';
            FilePath[1] = '/';
        }
    #endif
    
    
    strncpy(FilePath+offset, _FilePath, len);
    if (!is_suffix(_FilePath, get_module_ext()))
    {
        unsigned slen = strlen(suffix);
        for(int i = len; i < len+slen; i++)
        {
            FilePath[i+offset] = suffix[i-len];
        }
    }
    
    Pxf::SharedLibrary* lib = new Pxf::SharedLibrary();
    
    if(!lib->Load(FilePath))
    {
        Message("Kernel", "File not found: '%s'", FilePath);
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
    Pxf::Message("Kernel", "Loaded '%s' (0x%x)", FilePath, module);
    
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
                Message("Kernel", "'%s' is overriding built-in '%s'.", FilePath, module->GetIdentifier());
                
                // Remove built-in module
                m_AvailableModules[i]->destroy(m_AvailableModules[i]->module);
                if (m_AvailableModules[i]->dynlib)
                {
                    delete m_AvailableModules[i]->dynlib;
                    m_AvailableModules[i] = 0;
                }
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

bool Pxf::Kernel::RegisterSystem(const char* _ModuleID, Pxf::System::SystemType _SystemType, unsigned _Identifier)
{
    for(int i = 0; i < m_AvailableModules.size(); i++)
    {
        Pxf::Module* mod = m_AvailableModules[i]->module;
        if (strcmp(mod->GetIdentifier(), _ModuleID) == 0)
        {
            return mod->RegisterSystem(this, _SystemType, _Identifier);
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