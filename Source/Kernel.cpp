#include <Pxf/Kernel.h>
#include <Pxf/Module.h>
#include <Pxf/Base/Config.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Logger.h>
#include <Pxf/Base/Memory.h>
#include <Pxf/Base/SharedLibrary.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Audio/NullAudioDevice.h>
#include <Pxf/Input/NullInputDevice.h>
#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Network/NetworkDevice.h>
#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/ResourceLoader.h>

#include <cstdio>
#include <cstdarg>

Pxf::Kernel* Pxf::Kernel::s_Kernel = 0;
const unsigned Pxf::Kernel::KERNEL_VERSION = PXF_PACKSHORT2(1, 3);

Pxf::Kernel::Kernel()
	: m_AudioDevice(0)
	, m_InputDevice(0)
	, m_GraphicsDevice(0)
	, m_ResourceManager(0)
	, m_NetworkDevice(0)
	, m_Loggers(0)
	, m_KernelTag(0)
	, m_LogTagCount(0)
	, m_LogTags(0)
	, m_MaxLogTags(64)
{
	s_Kernel = this;
	m_Loggers = new LoggerEntry_t(new StdLogger());

	if (!m_LogTags)
	{
		m_LogTags = new const char*[m_MaxLogTags];
		MemoryZero(m_LogTags, m_MaxLogTags*sizeof(char*));
		m_LogTags[0] = "unk";
		m_LogTagCount = 1;
	}

	m_KernelTag = CreateTag("krn");

	// We need to make sure that the resource manager is created in this address space.
	Resource::ResourceManager* mgr = GetResourceManager();
}

Pxf::Kernel::~Kernel()
{

	if (m_AudioDevice)
		delete m_AudioDevice;

	if (m_InputDevice)
		delete m_InputDevice;
	
	if (m_GraphicsDevice)
		delete m_GraphicsDevice;

	if (m_NetworkDevice)
		delete m_NetworkDevice;

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

	if (m_Loggers)
	{
		while(m_Loggers)
		{
			SafeDelete(m_Loggers->logger);
			if (m_Loggers->next == 0)
				break;

			m_Loggers->logger = m_Loggers->next->logger;
			m_Loggers->next = m_Loggers->next->next;
		}
		SafeDelete(m_Loggers);
	}

}


void Pxf::Kernel::RegisterAudioDevice(Pxf::Audio::AudioDevice* _Device)
{
	if (_Device == 0)
		m_AudioDevice = new Pxf::Audio::NullAudioDevice(this);
	else
		m_AudioDevice = _Device;
	Log(m_KernelTag, "Registered audio device '%s'", m_AudioDevice->GetIdentifier());
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
	Log(m_KernelTag, "Registered input device '%s'", m_InputDevice->GetIdentifier());
}

Pxf::Input::InputDevice* Pxf::Kernel::GetInputDevice()
{
	if (!m_InputDevice)
		RegisterInputDevice(0);
	return m_InputDevice;
}
		
void Pxf::Kernel::RegisterGraphicsDevice(Pxf::Graphics::GraphicsDevice* _Device)
{
	Log(m_KernelTag, "Registering video device '%s'", _Device->GetIdentifier());
	m_GraphicsDevice = _Device;
}

Pxf::Graphics::GraphicsDevice* Pxf::Kernel::GetGraphicsDevice()
{
	return m_GraphicsDevice;
}

void Pxf::Kernel::RegisterNetworkDevice(Pxf::Network::NetworkDevice* _Device)
{
	Log(m_KernelTag, "Registering network device '%s'", _Device->GetIdentifier());
	m_NetworkDevice = _Device;
}

Pxf::Network::NetworkDevice* Pxf::Kernel::GetNetworkDevice()
{
	return m_NetworkDevice;
}

void Pxf::Kernel::RegisterResourceLoader(const char* _Ext, Resource::ResourceLoader* _ResourceLoader)
{
	Resource::ResourceManager* mgr = GetResourceManager();
	mgr->RegisterResourceLoader(_Ext, _ResourceLoader);
}

Pxf::Resource::ResourceManager* Pxf::Kernel::GetResourceManager()
{
	if (!m_ResourceManager)
		m_ResourceManager = new Pxf::Resource::ResourceManager(this);
	return m_ResourceManager;
}

void Pxf::Kernel::RegisterLogger(Logger* _logger)
{
	LoggerEntry_t* iter = m_Loggers;
	while(iter)
	{
		if (iter->logger == _logger)
			break; /* already registered */
		else if (iter->next == 0)
		{
			LoggerEntry_t* log = new LoggerEntry_t(_logger);
			iter->next = log;
			break;
		}
		iter = iter->next;
	}
}

void Pxf::Kernel::UnregisterLogger(Logger* _logger)
{
	LoggerEntry_t* iter = m_Loggers;
	while(iter)
	{
		if (iter->logger == _logger)
		{
			SafeDelete(iter->logger);
			if (iter->next)
			{
				LoggerEntry_t* old = iter->next;
				iter->logger = iter->next->logger;
				iter->next = iter->next->next;
				SafeDelete(old);
			}
			else
				iter->next = 0;
		}

		if (iter->next == 0)
			break;
		iter = iter->next;
	}
}

void Pxf::Kernel::Log(unsigned int _Tag, const char* _Message, ...)
{
	// If _Tag has no warning level flag, set debug-flag
	if (_Tag & 0xF0000000 == 0)
		_Tag |= Logger::IS_DEBUG;

	// Write to loggers	
	char Buffer[4092];

	va_list va;
	va_start(va, _Message);
	vsprintf(Buffer, _Message, va);
	va_end(va);

	LoggerEntry_t* iter = m_Loggers;
	while(iter)
	{
		iter->logger->Write(_Tag, m_LogTags, m_MaxLogTags, Buffer);
		if (iter->next == 0)
			break;
		iter = iter->next;
	}

}

unsigned int Pxf::Kernel::CreateTag(const char* _TagName)
{
	unsigned check = FindTagID(_TagName);
	if (check != 0)
		return check | Logger::IS_REGISTERED;

	unsigned ret = m_LogTagCount++;
	m_LogTags[ret] = _TagName;
	ret |= Logger::IS_REGISTERED;
	return ret;
}

unsigned int Pxf::Kernel::FindTagID(const char* _TagName)
{
	for(int i = 0; i < m_MaxLogTags; i++)
	{
		if (m_LogTags[i] && StringCompareI(_TagName, m_LogTags[i]) == 0)
			return i;
	}
	return 0;
}

static const char* get_full_module_ext()
{
	#ifdef CONF_DEBUG
		#define MODULE_SUFFIX "_ddyn"
	#else
		#define MODULE_SUFFIX "_s"
	#endif
	
	#define RETVAL MODULE_SUFFIX CONF_DYLIB_EXT
	return RETVAL;
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
	if (!Pxf::IsSuffix(_FilePath, CONF_DYLIB_EXT))
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
		const char* error = lib->GetError();
		if (error != 0x0)
		{
			char buffer[4096];
			size_t len = StringLength(error);
			StringCopy(buffer, error, len);
			for (int i = len; i--;)
			{
				if (buffer[i] == '\n' || buffer[i] == '\r')
					buffer[i] = 0x00;
			}
			Log(m_KernelTag, "Error loading '%s' => '%s'", FilePath, buffer);
		}
		else
		{
			Log(m_KernelTag, "Error loading '%s' => 'unknown'", FilePath);
		}
		return false;
	}
	
	typedef Pxf::Module*(*CreateInstance_fun)(void);
		
	CreateInstance_fun CreateInstance = (CreateInstance_fun)lib->LookupName("CreateInstance");
	DestroyModuleInstance_fun DestroyInstance = (DestroyModuleInstance_fun)lib->LookupName("DestroyInstance");
	
	if(!CreateInstance)
	{
		Log(m_KernelTag, "CreateInstance: %x", CreateInstance);
		delete lib;
		return false;
	}
	
	Pxf::Module* module = CreateInstance();
	Log(m_KernelTag | Logger::IS_DEBUG, "Loaded '%s'", FilePath);
	
	// Check that the module isn't already available, or override
	bool replaced = false;
	for(int i = 0; i < m_AvailableModules.size(); i++)
	{
		if (StringCompare(m_AvailableModules[i]->module->GetIdentifier(), module->GetIdentifier()) == 0)
		{
			if (!_OverrideBuiltin)
			{
				Log(m_KernelTag | Logger::IS_DEBUG, "Module '%s' is already built-in.", module->GetIdentifier());
				DestroyInstance(module);
				delete lib;
				return false;
			}
			else
			{
				Log(m_KernelTag | Logger::IS_DEBUG, "'%s' is overriding built-in '%s'.", FilePath, module->GetIdentifier());
				
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
		Log(m_KernelTag | Logger::IS_WARNING, "Warning, kernel version mismatch (%d.%d is recommended)", currkmaj, currkmin);
	}
	
	unsigned short currmmaj, currmmin;
	Pxf::UnpackShort2(Pxf::Module::MODULE_VERSION, &currmmaj, &currmmin);
	if (mmaj < currmmaj || (mmaj == currmmaj && mmin < currmmin))
	{
		Log(m_KernelTag | Logger::IS_WARNING, "Warning - Module API version mismatch (%d.%d is recommended)", currmmaj, currmmin);
	}
	
	//Log(m_KernelTag | Logger::IS_DEBUG, "Registered %s (dylib, kv: %d.%d, mv: %d.%d) to kernel %x", module->GetIdentifier(), kmaj, kmin, mmaj, mmin, this);
	
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

bool Pxf::Kernel::RegisterModule(Pxf::Module* _Module)
{
	m_AvailableModules.push_back(new ModuleEntry_t(_Module, DestroyBuiltInInstance));
	
	// TODO: Need to be able to specify which parts of the built-ins to use...
	_Module->RegisterSystem(this, 0xFFFFFFFF);
	return true;
}

void Pxf::Kernel::DumpAvailableModules()
{
	Log(m_KernelTag, "Dumping available modules:");
	for(int i = 0; i < m_AvailableModules.size(); i++)
	{
		const char* path = "built-in";
		if (m_AvailableModules[i]->dynlib)
			path = m_AvailableModules[i]->dynlib->GetFilePath();
		Log(m_KernelTag, "| %d. %s (%s)", i, m_AvailableModules[i]->module->GetIdentifier(), path);
	}
}
