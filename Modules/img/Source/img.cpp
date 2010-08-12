#include <Pxf/Base/Config.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Kernel.h>
#include <Pxf/System.h>
#include <Pxf/Module.h>
#include <Pxf/Modules/img/img.h>
#include <Pxf/Modules/img/ImageLoader.h>


static const unsigned Module_Kernel_Version = Pxf::Kernel::KERNEL_VERSION;
static const unsigned Module_Api_Version = Pxf::Module::MODULE_VERSION;

using Pxf::Modules::GenericImageImporter;
REGISTER_MODULE(GenericImageImporter, "img", Module_Kernel_Version, Module_Api_Version);

namespace GenericImageLoader_
{
    PXFEXPORT Pxf::Module* CreateInstance()
    {
        Pxf::Modules::GenericImageImporter *m = new Pxf::Modules::GenericImageImporter("img", Module_Kernel_Version, Module_Api_Version);
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

bool Pxf::Modules::GenericImageImporter::RegisterSystem(Pxf::Kernel* _Kernel, unsigned _SystemType, unsigned _Identifier)
{
    if (_SystemType == Pxf::System::SYSTEM_TYPE_RESOURCE_LOADER)
    {
        Pxf::Modules::GenericImageLoader* loader = new Pxf::Modules::GenericImageLoader(_Kernel);
        _Kernel->RegisterResourceLoader("png", loader);
        _Kernel->RegisterResourceLoader("jpg", loader);
        _Kernel->RegisterResourceLoader("jpeg", loader);
        return true;
    }
    
    return false;
}