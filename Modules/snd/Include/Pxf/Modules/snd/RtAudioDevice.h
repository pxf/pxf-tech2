#ifndef _PXF_MODULES_RTAUDIODEVICE_H_
#define _PXF_MODULES_RTAUDIODEVICE_H_

#include <Pxf/Audio/AudioDevice.h>
#include <Pxf/Base/Debug.h>

#include <RtAudio.h>

namespace Pxf
{
    namespace Modules
    {
        class RtAudioDevice : public Pxf::Audio::AudioDevice
        {
        private:
            virtual bool Init()
            {
				RtAudio dac;
				Message("Audio", "Number of output devices: %d", dac.getDeviceCount());
				return true;
			}
        public:
        RtAudioDevice(Pxf::Kernel* _Kernel)
            : Pxf::Audio::AudioDevice(_Kernel, "Rt Audio Device")
        {
			Init();
		}
        
        virtual void Play(unsigned int _SoundID)
        {}
        virtual void Stop(unsigned int _SoundID)
        {}
        virtual void StopAll()
        {}
        virtual void Pause(unsigned int _SoundID)
        {}
        virtual void PauseAll()
        {}
        };
    }
}

#endif // _PXF_MODULES_RTAUDIODEVICE_H_