#ifndef _PXF_AUDIODEVICE_AUDIODEVICE_H_
#define _PXF_AUDIODEVICE_AUDIODEVICE_H_

#include <Pxf/Base/Types.h>
#include <Pxf/System.h>

namespace Pxf
{
    class Kernel;
	namespace Resource
	{
		class Sound;
	}

    namespace Audio
    {
        class AudioDevice : public Pxf::System
        {
        public:
        AudioDevice(Kernel* _Kernel, const char* _Identifier)
            : Pxf::System(_Kernel, Pxf::System::SYSTEM_TYPE_AUDIODEVICE, _Identifier)
        {}

		virtual ~AudioDevice(){};

		virtual bool Initialize(){ return false; };
        
		virtual int RegisterSound(const char* _Filename) = 0;
        virtual int RegisterSound(Resource::Sound* _Sound) = 0;
		virtual int GetSoundID(const Resource::Sound* _Sound) = 0;
		virtual void UnregisterSound(int _Id) = 0;
		inline void UnregisterSound(const Resource::Sound* _Sound)
		{
			UnregisterSound(GetSoundID(_Sound));
		}

        virtual void Play(int _SoundID, bool _Loop = false) = 0;
        virtual void Stop(int _SoundID) = 0;
        virtual void StopAll() = 0;
        virtual void Pause(int _SoundID) = 0;
        virtual void PauseAll() = 0;
		virtual void DumpInfo() = 0;
        };
    } // Audio
} // Pxf

#endif // _PXF_AUDIODEVICE_AUDIODEVICE_H_