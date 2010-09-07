#ifndef _PXF_MODULES_RTAUDIODEVICE_H_
#define _PXF_MODULES_RTAUDIODEVICE_H_

#include <Pxf/Audio/AudioDevice.h>
#include <Pxf/Base/Debug.h>

class RtAudio;

namespace Pxf
{
    namespace Modules
    {
        class RtAudioDevice : public Pxf::Audio::AudioDevice
        {
        private:
			RtAudio* m_DAC;

			unsigned int m_Channels;

            virtual bool Init();
        public:
        RtAudioDevice(Pxf::Kernel* _Kernel)
            : Pxf::Audio::AudioDevice(_Kernel, "Rt Audio Device")
        {
			Init();
		}

		virtual ~RtAudioDevice();

		virtual int RegisterSound(const Resource::Sound* _Sound);
		virtual int GetSoundID(const Resource::Sound* _Sound);
		virtual void UnregisterSound(int _Id);
        virtual void Play(unsigned int _SoundID);
        virtual void Stop(unsigned int _SoundID);
        virtual void StopAll();
        virtual void Pause(unsigned int _SoundID);
        virtual void PauseAll();

        };
    }
}

#endif // _PXF_MODULES_RTAUDIODEVICE_H_