#include <Pxf/Base/Config.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Kernel.h>
#include <Pxf/Module.h>
#include <Pxf/SampleModule/SampleModule.h>

static const unsigned Module_Kernel_Version = Pxf::Kernel::KERNEL_VERSION;
static const unsigned Module_Api_Version = Pxf::Module::MODULE_VERSION;

using Pxf::Modules::SampleModule;
REGISTER_MODULE(SampleModule, Module_Kernel_Version, Module_Api_Version);


namespace Aoeaoe
{
PXFEXPORT Pxf::Module* CreateInstance()
{
    Pxf::Modules::SampleModule *m = new Pxf::Modules::SampleModule("SampleModule", Module_Kernel_Version, Module_Api_Version);
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


bool Pxf::Modules::SampleModule::RegisterSystem(Pxf::Kernel* _Kernel, unsigned _SystemType)
{
    Message("SampleModule", "Checking module for system type %d", _SystemType);
    return false;
}