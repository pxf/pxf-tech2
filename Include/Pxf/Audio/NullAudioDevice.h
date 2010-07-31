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