#ifndef _PXF_AUDIODEVICE_AUDIODEVICE_H_
#define _PXF_AUDIODEVICE_AUDIODEVICE_H_

#include <Pxf/Base/Types.h>
#include <Pxf/System.h>

namespace Pxf
{
    class Kernel;
    namespace Audio
    {
        class AudioDevice : public Pxf::System
        {
        public:
        AudioDevice(Kernel* _Kernel, const char* _Identifier)
            : Pxf::System(_Kernel, Pxf::System::SYSTEM_TYPE_AUDIODEVICE, _Identifier)
        {}
        
        //unsigned RegisterSound(const Resource::Sound* _Sound) = 0;
        virtual void Play(unsigned int _SoundID) = 0;
        virtual void Stop(unsigned int _SoundID) = 0;
        virtual void StopAll() = 0;
        virtual void Pause(unsigned int _SoundID) = 0;
        virtual void PauseAll() = 0;
        
        private:
        };
    } // Audio
} // Pxf

#endif // _PXF_AUDIODEVICE_AUDIODEVICE_H_