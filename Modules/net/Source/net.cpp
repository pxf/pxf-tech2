#include <Pxf/Base/Config.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Kernel.h>
#include <Pxf/System.h>
#include <Pxf/Module.h>
#include <Pxf/Modules/net/net.h>
#include <Pxf/Modules/net/ENetDevice.h>


static const unsigned Module_Kernel_Version = Pxf::Kernel::KERNEL_VERSION;
static const unsigned Module_Api_Version = Pxf::Module::MODULE_VERSION;

using Pxf::Modules::NetworkLoader;

#ifndef CONF_MODULAR
REGISTER_MODULE(NetworkLoader, "net", Module_Kernel_Version, Module_Api_Version);
#endif

namespace NetworkLoader_
{
    PXFEXPORT Pxf::Module* CreateInstance()
    {
        Pxf::Modules::NetworkLoader *m = new Pxf::Modules::NetworkLoader("net", Module_Kernel_Version, Module_Api_Version);
        return m;
    }

    PXFEXPORT void DestroyInstance(Pxf::Module* _module)
    {
        if (_module)
        {
            delete _module;
        }
    }
}

bool Pxf::Modules::NetworkLoader::RegisterSystem(Pxf::Kernel* _Kernel, unsigned _SystemType)
{
	bool retval = false;
	if (_SystemType & Pxf::System::SYSTEM_TYPE_NETWORKDEVICE)
	{
		Pxf::Modules::ENetDevice* device = new Pxf::Modules::ENetDevice(_Kernel);
		_Kernel->RegisterNetworkDevice(device);
		retval = true;
	}
    
    return retval;
}
