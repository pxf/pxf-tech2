#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/SharedLibrary.h>
#include <Pxf/Module.h>
#include <Pxf/Base/Utils.h>

#include <Pxf/Audio/NullAudioDevice.h>
#include <Pxf/Input/NullInputDevice.h>

#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/ResourceLoader.h>

#include <Pxf/Base/String.h>

Pxf::Kernel* Pxf::Kernel::s_Kernel = 0;
const unsigned Pxf::Kernel::KERNEL_VERSION = PXF_PACKSHORT2(1, 1);

Pxf::Kernel::Kernel()
    : m_AudioDevice(0)
    , m_InputDevice(0)
    , m_GraphicsDevice(0)
    , m_ResourceManager(0)
{
    // We need to make sure that the resource manager is created in this address space.
	s_Kernel = this;
    Resource::ResourceManager* mgr = GetResourceManager();
}

Pxf::Kernel::~Kernel()
{

	if (m_AudioDevice)
		delete m_AudioDevice;

    if (m_ResourceManager)
        delete m_ResourceManager;

    for(int i = 0; i < m_AvailableModules.size(); i++)
    {
        Pxf::Module* m = m_AvailableModules[i]->module;
        m_AvailableModules[i]->destroy(m);
        if (m_AvailableModules[i]->dynlib)
            delete m_AvailableModules[i]->dynlib;
        delete m_AvailableModules[i];
    }
}


void Pxf::Kernel::RegisterAudioDevice(Pxf::Audio::AudioDevice* _Device)
{
	if (_Device == 0)
		m_AudioDevice = new Pxf::Audio::NullAudioDevice(this);
	else
		m_AudioDevice = _Device;
    Pxf::Message("Kernel", "Registered audio device '%s'", m_AudioDevice->GetIdentifier());
}

Pxf::Audio::AudioDevice* Pxf::Kernel::GetAudioDevice()
{
    if (!m_AudioDevice)
        RegisterAudioDevice(0);
    return m_AudioDevice;
}

void Pxf::Kernel::RegisterInputDevice(Pxf::Input::InputDevice* _Device)
{
	if (_Device == 0)
		m_InputDevice = new Pxf::Input::NullInputDevice(this);
	else
		m_InputDevice = _Device;
    Pxf::Message("Kernel", "Registered input device '%s'", m_InputDevice->GetIdentifier());
}

Pxf::Input::InputDevice* Pxf::Kernel::GetInputDevice()
{
    if (!m_InputDevice)
		RegisterInputDevice(0);
    return m_InputDevice;
}
        
void Pxf::Kernel::RegisterGraphicsDevice(Pxf::Graphics::GraphicsDevice* _Device)
{
    Pxf::Message("Kernel", "Registering video device '%s'", _Device->GetIdentifier());
    m_GraphicsDevice = _Device;
}

Pxf::Graphics::GraphicsDevice* Pxf::Kernel::GetGraphicsDevice()
{
    //if (!m_GraphicsDevice)
    //    m_GraphicsDevice = new Pxf::Graphics::NullGraphicsDevice(this);
    return m_GraphicsDevice;
}

void Pxf::Kernel::RegisterResourceLoader(const char* _Ext, Resource::ResourceLoader* _ResourceLoader)
{
    Resource::ResourceManager* mgr = GetResourceManager();
    mgr->RegisterResourceLoader(_Ext, _ResourceLoader);
}

Pxf::Resource::ResourceManager* Pxf::Kernel::GetResourceManager()
{
    if (!m_ResourceManager)
        m_ResourceManager = new Pxf::Resource::ResourceManager();
    return m_ResourceManager;
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

bool Pxf::Kernel::RegisterModule(const char* _FilePath, unsigned _Filter, bool _OverrideBuiltin)
{
    // If the file is missing extention, add one that's appropriate for the platform.
    const char* suffix = get_full_module_ext();
    char FilePath[256] = {0};
    unsigned len = StringLength(_FilePath);
    unsigned offset = 0;
    
// TODO: Redo this
#if defined(CONF_PLATFORM_MACOSX)
    if (!Pxf::IsPrefix(_FilePath, "./"))
    {
        offset = 5;
        StringCopy(FilePath, "./lib", 5);
    }
#elif defined(CONF_FAMILY_UNIX)
    if (!Pxf::IsPrefix(_FilePath, "./"))
    {
        offset = 2;
        StringCopy(FilePath, "./", 5);
    }
#endif
    
    
    StringCopy(FilePath+offset, _FilePath, len);
    if (!Pxf::IsSuffix(_FilePath, get_module_ext()))
    {
        unsigned slen = StringLength(suffix);
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
        if (StringCompare(m_AvailableModules[i]->module->GetIdentifier(), module->GetIdentifier()) == 0)
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

	module->RegisterSystem(this, _Filter);

    return true;
}

static void DestroyBuiltInInstance(Pxf::Module* _Module)
{
    if (_Module)
        delete _Module;
}

// FIXME: Currently disabled, it should be used by the global initializers when compiling statically
// However, only dynamically compiled libraries seems to be calling... sup?
bool Pxf::Kernel::RegisterModule(Pxf::Module* _Module)
{
//    m_AvailableModules.push_back(new ModuleEntry_t(_Module, DestroyBuiltInInstance));
	
	// TODO: Need to be able to specify which parts of the built-ins to use...
//	_Module->RegisterSystem(this, 0xFFFFFFFF);
    return true;
}

void Pxf::Kernel::DumpAvailableModules()
{
    Message("Kernel", "Dumping available modules:");
    for(int i = 0; i < m_AvailableModules.size(); i++)
    {
		const char* path = "built-in";
		if (m_AvailableModules[i]->dynlib)
			path = m_AvailableModules[i]->dynlib->GetFilePath();
        Message("Kernel", "| %d. \t%s (%s)", i, m_AvailableModules[i]->module->GetIdentifier(), path);
    }
}