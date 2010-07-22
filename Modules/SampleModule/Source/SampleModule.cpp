#include <Pxf/Base/Config.h>
#include <Pxf/Base/Module.h>
#include <Pxf/Kernel.h>
#include <Pxf/SampleModule/SampleModule.h>

#define SAMPLE_MODULE_API_VERSION 2


static const unsigned Module_Kernel_Version = 0; // Pxf::Kernel::KERNEL_VERSION;
static const unsigned Module_Api_Version = 1; // Pxf::Kernel::GRAPHICS_API_VERSION
// Pxf::Kernel::RegisterModule(MODULE_TYPE_GRAPHICS, "module.dll")
// Pxf::Kernel::SetPreferredModule(MODULE_TYPE_GRAPHICS, "OPENGL2)

// TODO: Declare in header file if compiling statically.
// ??

PXFMODULE Pxf::Base::Module* CreateInstance()
{
    return new Pxf::Modules::SampleModule(Module_Kernel_Version, Module_Api_Version);
}

PXFMODULE void DestroyInstance(Pxf::Base::Module* _module)
{
    delete _module;
}

void Pxf::Modules::SampleModule::RegisterSystems(Pxf::Kernel* _Kernel)
{
    Message("SampleModule", "Registering all available systems to kernel %x", _Kernel);
}