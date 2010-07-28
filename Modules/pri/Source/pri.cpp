#include <Pxf/Base/Config.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Kernel.h>
#include <Pxf/Module.h>
#include <Pxf/Modules/pri/pri.h>

#include <Pxf/Modules/pri/DeviceGL2.h>

static const unsigned Module_Kernel_Version = Pxf::Kernel::KERNEL_VERSION;
static const unsigned Module_Api_Version = Pxf::Module::MODULE_VERSION;

using Pxf::Modules::PortableRendererInput;
REGISTER_MODULE(PortableRendererInput, Module_Kernel_Version, Module_Api_Version);

namespace PortableRenderInput_
{
    PXFEXPORT Pxf::Module* CreateInstance()
    {
        Pxf::Modules::PortableRendererInput *m = new Pxf::Modules::PortableRendererInput("PortableRendererInput", Module_Kernel_Version, Module_Api_Version);
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

bool Pxf::Modules::PortableRendererInput::RegisterSystem(Pxf::Kernel* _Kernel, unsigned _SystemType, unsigned _Identifier)
{
    Message("PortableRendererInput", "Checking module for system type %d, identifier = %d", _SystemType, _Identifier);
    
    if (_SystemType == Pxf::Kernel::SYSTEM_TYPE_GRAPHICS)
    {
        //_Kernel->RegisterSystem(_SystemType, new Pxf::Modules::pri::GraphicsDeviceGL2());
        return true;
    }
    
    return false;
}