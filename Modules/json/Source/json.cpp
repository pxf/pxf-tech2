#include <Pxf/Base/Config.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Kernel.h>
#include <Pxf/System.h>
#include <Pxf/Module.h>
#include <Pxf/Modules/json/json.h>
#include <Pxf/Modules/json/JsonLoader.h>

static const unsigned Module_Kernel_Version = Pxf::Kernel::KERNEL_VERSION;
static const unsigned Module_Api_Version = Pxf::Module::MODULE_VERSION;

using Pxf::Modules::JsonImporter;
#ifndef CONF_MODULAR
REGISTER_MODULE(JsonImporter, "json", Module_Kernel_Version, Module_Api_Version);
#endif

namespace CtmMeshLoader_
{
    PXFEXPORT Pxf::Module* CreateInstance()
    {
        Pxf::Modules::JsonImporter *m = new Pxf::Modules::JsonImporter("json", Module_Kernel_Version, Module_Api_Version);
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

bool Pxf::Modules::JsonImporter::RegisterSystem(Pxf::Kernel* _Kernel, unsigned _SystemType)
{
    if (_SystemType & Pxf::System::SYSTEM_TYPE_RESOURCE_LOADER)
    {
        Pxf::Modules::JsonLoader* loader = new Pxf::Modules::JsonLoader(_Kernel);
        _Kernel->RegisterResourceLoader("json", loader);
        return true;
    }
    
    return false;
}