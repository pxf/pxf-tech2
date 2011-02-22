#include <Pxf/Base/Config.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Kernel.h>
#include <Pxf/System.h>
#include <Pxf/Module.h>
#include <Pxf/Modules/iphone/iphone.h>

#include <Pxf/Modules/iphone/DeviceGLES.h>
#include <Pxf/Modules/iphone/InputDeviceIOS.h>

static const unsigned Module_Kernel_Version = Pxf::Kernel::KERNEL_VERSION;
static const unsigned Module_Api_Version = Pxf::Module::MODULE_VERSION;

// Add static initializer for modules if we are embedding modules
using Pxf::Modules::PortableRendererInput;
#ifndef CONF_MODULAR
REGISTER_MODULE(PortableRendererInput, "iphone", Module_Kernel_Version, Module_Api_Version);
#endif

// Avoid name collisions with other modules
namespace PortableRenderInput_
{
	PXFEXPORT Pxf::Module* CreateInstance()
	{
		Pxf::Modules::PortableRendererInput *m = new Pxf::Modules::PortableRendererInput("iphone", Module_Kernel_Version, Module_Api_Version);
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

bool Pxf::Modules::PortableRendererInput::RegisterSystem(Pxf::Kernel* _Kernel, unsigned _SystemType)
{
	bool retval = false;

	if (_SystemType & Pxf::System::SYSTEM_TYPE_GRAPHICSDEVICE)
	{
		_Kernel->RegisterGraphicsDevice(new Pxf::Modules::DeviceGLES(_Kernel));
		retval = true;
	}

	if (_SystemType & Pxf::System::SYSTEM_TYPE_INPUTDEVICE)
	{
		_Kernel->RegisterInputDevice(new Pxf::Modules::InputDeviceIOS(_Kernel));
		retval = true;
	}
	
	return retval;
}