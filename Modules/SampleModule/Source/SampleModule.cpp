#include <Pxf/Base/Config.h>
#include <Pxf/Base/Module.h>
#include <Pxf/Kernel.h>
#include <Pxf/SampleModule/SampleModule.h>

#define SAMPLE_MODULE_API_VERSION 2



static const unsigned Module_API_Version = 1; // Pxf::Kernel::GRAPHICS_API_VERSION
static const unsigned Module_Kernel_Version = 0; // Pxf::Kernel::KERNEL_VERSION;
static const unsigned Module_Type = Pxf::Kernel::MODULE_TYPE_GRAPHICS;
// Pxf::Kernel::RegisterModule(MODULE_TYPE_GRAPHICS, "module.dll")
// Pxf::Kernel::SetPreferredModule(MODULE_TYPE_GRAPHICS, "OPENGL2)

// TODO: Declare in header file if compiling statically.

PXFMODULE unsigned GetEngineVersion()
{
    return Module_Kernel_Version;
}

PXFMODULE unsigned GetModuleVersion()
{
    return Module_API_Version;
}

PXFMODULE unsigned GetModuleType()
{
    return Module_Type;
}

PXFMODULE Pxf::Base::Module* CreateInstance()
{
    return new Pxf::Modules::SampleModule(SAMPLE_MODULE_API_VERSION);
}

PXFMODULE void DestroyInstance(Pxf::Base::Module* _module)
{
    delete _module;
}