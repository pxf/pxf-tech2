#include <Pxf/Base/Module.h>
#include <Pxf/Kernel.h>
#include <Pxf/SampleModule/SampleModule.h>

#define SAMPLE_MODULE_API_VERSION 2

static const unsigned SampleModule_API_Version = 1;
static const unsigned SampleModule_Engine_Version = 0; // Pxf::Kernel::GetEngineVersion();
static const unsigned SampleModule_Type = Pxf::Kernel::MODULE_TYPE_GRAPHICS;
// Pxf::Kernel::RegisterModule(MODULE_TYPE_GRAPHICS, "module.dll")
// Pxf::Kernel::SetPreferredModule(MODULE_TYPE_GRAPHICS, "OPENGL2)

extern "C" unsigned __declspec(dllexport) GetEngineVersion()
{
    return SampleModule_Engine_Version;
}

extern "C" unsigned __declspec(dllexport) GetModuleVersion()
{
    return SampleModule_API_Version;
}

extern "C" unsigned __declspec(dllexport) GetModuleType()
{
    return SampleModule_Type;
}

class SampleModule : public Pxf::Base::Module
{
    private:
    public:
    SampleModule(unsigned ApiVersion)
        : Module(ApiVersion)
    {}
};

extern "C" __declspec(dllexport) SampleModule* CreateInstance()
{
    return new SampleModule(SAMPLE_MODULE_API_VERSION);
}

extern "C" __declspec(dllexport) void DestroyInstance(SampleModule* _module)
{
    delete _module;
}