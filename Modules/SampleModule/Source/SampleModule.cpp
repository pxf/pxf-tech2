#include <Pxf/Base/Module.h>
#include <Pxf/SampleModule/SampleModule.h>

#define SAMPLE_MODULE_API_VERSION 1

static const int SampleModule_API_Version = 1;
static const int SampleModule_Engine_Version = 0; // Pxf::Kernel::GetEngineVersion();
static const int SampleModule_Type = 0; //Pxf::Kernel::MODULE_TYPE_GRAPHICS
// Pxf::Kernel::RegisterModule(MODULE_TYPE_GRAPHICS, "module.dll")
// Pxf::Kernel::SetPreferredModule(MODULE_TYPE_GRAPHICS, "OPENGL2)

extern "C" int __declspec(dllexport) lulz(int x)
{
    return 3;
}

extern "C" int __declspec(dllexport) GetEngineVersion();
extern "C" int __declspec(dllexport) GetModuleVersion();

//extern "C" Pxf::Base::Module<SampleModule>* __declspec(dllexport) CreateModule()
//{
//    return new Pxf::Base::Module<SampleModule>(SAMPLE_MODULE_API_VERSION);
//}