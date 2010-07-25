#include <Pxf/Base/Config.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Kernel.h>
#include <Pxf/Module.h>
#include <Pxf/SampleModule/SampleModule.h>

static const unsigned Module_Kernel_Version = 0; // Pxf::Kernel::KERNEL_VERSION;
static const unsigned Module_Api_Version = 1; // Pxf::Kernel::GRAPHICS_API_VERSION

#include <Pxf/Kernel.h>
using Pxf::Modules::SampleModule;
REGISTER_MODULE(SampleModule, Module_Kernel_Version, Module_Api_Version);


PXFEXPORT Pxf::Module* CreateInstance()
{
    return new Pxf::Modules::SampleModule("SampleModule", Module_Kernel_Version, Module_Api_Version);
}

PXFEXPORT void DestroyInstance(Pxf::Module* _module)
{
    delete _module;
}

void Pxf::Modules::SampleModule::RegisterSystems(Pxf::Kernel* _Kernel)
{
    Message("SampleModule", "Registering all available systems to kernel %x", _Kernel);
}