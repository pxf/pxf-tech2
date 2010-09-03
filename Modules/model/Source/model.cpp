#include <Pxf/Base/Config.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Kernel.h>
#include <Pxf/System.h>
#include <Pxf/Module.h>
#include <Pxf/Modules/model/model.h>
#include <Pxf/Modules/model/ModelLoader.h>

static const unsigned Module_Kernel_Version = Pxf::Kernel::KERNEL_VERSION;
static const unsigned Module_Api_Version = Pxf::Module::MODULE_VERSION;

using Pxf::Modules::GenericModelImporter;
#ifndef CONF_MODULAR
REGISTER_MODULE(GenericModelImporter, "model", Module_Kernel_Version, Module_Api_Version);
#endif

namespace GenericModelLoader_
{
    PXFEXPORT Pxf::Module* CreateInstance()
    {
        Pxf::Modules::GenericModelImporter *m = new Pxf::Modules::GenericModelImporter("model", Module_Kernel_Version, Module_Api_Version);
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

bool Pxf::Modules::GenericModelImporter::RegisterSystem(Pxf::Kernel* _Kernel, unsigned _SystemType)
{
    if (_SystemType & Pxf::System::SYSTEM_TYPE_RESOURCE_LOADER)
    {
        Pxf::Modules::GenericModelLoader* loader = new Pxf::Modules::GenericModelLoader(_Kernel);
        _Kernel->RegisterResourceLoader("ctm", loader);
        return true;
    }
    
    return false;
}