#include <Pxf/Base/Config.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Kernel.h>
#include <Pxf/System.h>
#include <Pxf/Module.h>
#include <Pxf/Modules/pri/pri.h>

#include <Pxf/Modules/pri/DeviceGL2.h>

static const unsigned Module_Kernel_Version = Pxf::Kernel::KERNEL_VERSION;
static const unsigned Module_Api_Version = Pxf::Module::MODULE_VERSION;

using Pxf::Modules::PortableRendererInput;
REGISTER_MODULE(PortableRendererInput, "pri", Module_Kernel_Version, Module_Api_Version);

namespace PortableRenderInput_
{
    PXFEXPORT Pxf::Module* CreateInstance()
    {
        Pxf::Modules::PortableRendererInput *m = new Pxf::Modules::PortableRendererInput("pri", Module_Kernel_Version, Module_Api_Version);
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

bool Pxf::Modules::PortableRendererInput::RegisterSystem(Pxf::Kernel* _Kernel, unsigned _SystemType)
{
    Message("PortableRendererInput", "Checking module for system type %d", _SystemType);
    
    if (_SystemType == Pxf::System::SYSTEM_TYPE_GRAPHICS)
    {
        _Kernel->RegisterGraphicsDevice(new Pxf::Modules::DeviceGL2(_Kernel));
        return true;
    }
    
    return false;
}