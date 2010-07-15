#include <Pxf/Base/Config.h>
#include <Pxf/Base/Module.h>
#include <Pxf/Kernel.h>
#include <Pxf/SampleModule/SampleModule.h>

#define SAMPLE_MODULE_API_VERSION 2

#ifdef CONF_FAMILY_WINDOWS
#define PXFDLLEXPORT __declspec(dllexport)
#else
#define PXFDLLEXPORT
#endif

static const unsigned SampleModule_API_Version = 1;
static const unsigned SampleModule_Engine_Version = 0; // Pxf::Kernel::GetEngineVersion();
static const unsigned SampleModule_Type = Pxf::Kernel::MODULE_TYPE_GRAPHICS;
// Pxf::Kernel::RegisterModule(MODULE_TYPE_GRAPHICS, "module.dll")
// Pxf::Kernel::SetPreferredModule(MODULE_TYPE_GRAPHICS, "OPENGL2)

extern "C" PXFDLLEXPORT unsigned GetEngineVersion()
{
    return SampleModule_Engine_Version;
}

extern "C" PXFDLLEXPORT unsigned GetModuleVersion()
{
    return SampleModule_API_Version;
}

extern "C" PXFDLLEXPORT unsigned GetModuleType()
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

extern "C" PXFDLLEXPORT SampleModule* CreateInstance()
{
    return new SampleModule(SAMPLE_MODULE_API_VERSION);
}

extern "C" PXFDLLEXPORT void DestroyInstance(SampleModule* _module)
{
    delete _module;
}