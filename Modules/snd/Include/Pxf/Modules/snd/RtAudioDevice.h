#ifndef _PXF_MODULES_RTAUDIODEVICE_H_
#define _PXF_MODULES_RTAUDIODEVICE_H_

#include <Pxf/Audio/AudioDevice.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Util/Array.h>

class RtAudio;

namespace Pxf
{
	namespace Modules
	{
		class RtAudioDevice : public Pxf::Audio::AudioDevice
		{
		public:
			struct SoundEntry
			{
				Resource::Sound* clip;
				unsigned int current_frame;
				bool active;
				bool loop;
				SoundEntry(Resource::Sound* _clip, unsigned int _current_frame, bool _active, bool _loop)
				{
					clip = _clip;
					current_frame = _current_frame;
					active = _active;
					loop = _loop;
				}
				SoundEntry()
				{
					clip = 0;
					current_frame = 0;
					active = false;
					loop = false;
				}
			};

		protected:
			RtAudio* m_DAC;
			Util::Array<Resource::Sound*> m_SoundBank;
			Util::Array<SoundEntry> m_ActiveVoices;
			unsigned int m_Channels;
			unsigned int m_BufferSize;
			unsigned int m_MaxVoices;
			unsigned int m_LogTag;

			bool m_Active;
			bool m_Initialized;
			bool m_Closed;

			void CloseStream();

			virtual bool Init() { return true; }
 
		public:
		RtAudioDevice(Pxf::Kernel* _Kernel);

		virtual bool Initialize(unsigned int _BufferSize = 1024, unsigned int _MaxVoices = 8);

		virtual ~RtAudioDevice();

		virtual int RegisterSound(const char* _Filename);
		virtual int RegisterSound(Resource::Sound* _Sound);
		virtual int GetSoundID(const Resource::Sound* _Sound);
		virtual void UnregisterSound(int _Id);
		virtual void Play(int _SoundID, bool _Loop);
		virtual void Stop(int _SoundID);
		virtual void StopAll();
		virtual void Pause(int _SoundID);
		virtual void PauseAll();
		virtual void DumpInfo();

		void _ShowMixerWarning(const char* _Msg);
		bool _IsClosed() {return m_Closed;}

		Util::Array<Resource::Sound*>* GetSoundBank()
		{
			return &m_SoundBank;
		}

		Util::Array<SoundEntry>* GetVoices()
		{
			return &m_ActiveVoices;
		}
		
		bool IsActive() const
		{
			return m_Active;
		}

		};
	}
}

#endif // _PXF_MODULES_RTAUDIODEVICE_H_