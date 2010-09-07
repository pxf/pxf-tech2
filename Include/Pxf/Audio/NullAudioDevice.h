#ifndef _PXF_AUDIODEVICE_NULLAUDIODEVICE_H_
#define _PXF_AUDIODEVICE_NULLAUDIODEVICE_H_

#include <Pxf/Audio/AudioDevice.h>
#include <Pxf/Base/Debug.h>

namespace Pxf
{
    namespace Audio
    {
        class NullAudioDevice : public Pxf::Audio::AudioDevice
        {
        private:
            virtual bool Init()
            {   return true;    }
        public:
        NullAudioDevice(Pxf::Kernel* _Kernel)
            : Pxf::Audio::AudioDevice(_Kernel, "Null Audio Device")
        {}

		virtual ~NullAudioDevice(){};

		virtual int RegisterSound(const Resource::Sound* _Sound)
		{
			return -1;
		}

		virtual int GetSoundID(const Resource::Sound* _Sound)
		{
			return -1;
		}

		virtual void UnregisterSound(int _Id)
		{

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

#endif // _PXF_AUDIODEVICE_NULLAUDIODEVICE_H_