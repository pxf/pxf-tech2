#include <Pxf/Base/Config.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Kernel.h>
#include <Pxf/System.h>
#include <Pxf/Module.h>
#include <Pxf/Modules/snd/snd.h>
#include <Pxf/Modules/snd/OggAudioLoader.h>
#include <Pxf/Modules/snd/RtAudioDevice.h>


static const unsigned Module_Kernel_Version = Pxf::Kernel::KERNEL_VERSION;
static const unsigned Module_Api_Version = Pxf::Module::MODULE_VERSION;

using Pxf::Modules::AudioImporter;
#ifndef CONF_MODULAR
REGISTER_MODULE(AudioImporter, "snd", Module_Kernel_Version, Module_Api_Version);
#endif

namespace AudioLoader_
{
	PXFEXPORT Pxf::Module* CreateInstance()
	{
		Pxf::Modules::AudioImporter *m = new Pxf::Modules::AudioImporter("snd", Module_Kernel_Version, Module_Api_Version);
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

bool Pxf::Modules::AudioImporter::RegisterSystem(Pxf::Kernel* _Kernel, unsigned _SystemType)
{
	bool retval = false;
	if (_SystemType & Pxf::System::SYSTEM_TYPE_AUDIODEVICE)
	{
		Pxf::Modules::RtAudioDevice* device = new Pxf::Modules::RtAudioDevice(_Kernel);
		_Kernel->RegisterAudioDevice(device);
		retval = true;
	}
	if (_SystemType & Pxf::System::SYSTEM_TYPE_RESOURCE_LOADER)
	{
		Pxf::Modules::OggAudioLoader* oggloader = new Pxf::Modules::OggAudioLoader(_Kernel);
		_Kernel->RegisterResourceLoader("ogg", oggloader);
		retval = true;
	}
	
	return retval;
}