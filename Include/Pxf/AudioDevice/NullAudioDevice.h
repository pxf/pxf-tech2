#ifndef _PXF_AUDIODEVICE_AUDIODEVICENULL_H_
#define _PXF_AUDIODEVICE_AUDIODEVICENULL_H_

#include <Pxf/AudioDevice/AudioDevice.h>
#include <Pxf/Base/Debug.h>

namespace Pxf
{
    class NullAudioDevice : public Pxf::AudioDevice
    {
    private:
        virtual bool Init()
        {   return true;    }
    public:
    NullAudioDevice(Kernel* _Kernel)
        : Pxf::AudioDevice(_Kernel, "Null Audio Device")
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

#endif // _PXF_AUDIODEVICE_AUDIODEVICENULL_H_