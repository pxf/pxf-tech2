#include <Pxf/Modules/snd/RtAudioDevice.h>
#include <Pxf/Kernel.h>
#include <Pxf/Math/Math.h>

#include <Pxf/Resource/Sound.h>
#include <Pxf/Resource/ResourceManager.h>

#include <RtAudio.h>

using namespace Pxf;
using namespace Pxf::Modules;

#define MAX_REGISTERED_SOUNDS 128

int mix(void *_outbuff, void *_inbuff, unsigned int _num_frames,
		double _time, RtAudioStreamStatus _status, void *_device)
{
	if (_status == RTAUDIO_OUTPUT_UNDERFLOW)
	{
		Message("Mixer", "Warning: underflow, stuttering might occur...");
	}
	
	RtAudioDevice* device = (RtAudioDevice*)_device;
	short* out = (short*)_outbuff;
	Util::Array<RtAudioDevice::SoundEntry>* voices = device->GetVoices();
	unsigned int num_voices = voices->size();
	
	RtAudioDevice::SoundEntry* entry;
	short* dataptr = 0;
	for(unsigned int i = 0; i < _num_frames*2; i += 2)
	{
		out[i] = 0;
		out[i+1] = 0;
		
		for (int j = 0; j < num_voices; j++)
		{
			entry = &voices->at(j);
			if (entry->clip && entry->active)
			{
				if (entry->current_frame >= entry->clip->DataLen())
				{
					entry->current_frame = 0;
					if (!entry->loop)
					{
						entry->clip = 0;
						continue;
					}
				}
				dataptr = entry->clip->DataPtr();
				out[i] += dataptr[entry->current_frame];
				out[i+1] += dataptr[entry->current_frame + 1];
				entry->current_frame += 2;
			}
		}
	}

	// Continue if active, else abort stream.
	return device->IsActive() ? 0 : 2;
}

bool RtAudioDevice::Initialize(unsigned int _BufferSize, unsigned int _MaxVoices)
{
	if (m_Initialized)
	{
		CloseStream();
		delete m_DAC;
	}

	m_BufferSize = _BufferSize;
	m_MaxVoices = _MaxVoices;

	m_SoundBank.resize(MAX_REGISTERED_SOUNDS);
	m_ActiveVoices.resize(m_MaxVoices);

	m_DAC = new RtAudio();
	m_DAC->showWarnings(true);

	unsigned num_devices = m_DAC->getDeviceCount();

	/* Initialize audio stream */
	if (num_devices == 0)
	{
		Message("Audio", "No available output devices");
		return false;
	}

	// Just assume this for now, would be nice to override somehow
	RtAudio::StreamParameters params;
	params.deviceId = m_DAC->getDefaultOutputDevice();
	params.nChannels = 2;
	params.firstChannel = 0;

	unsigned int buffer_frames = _BufferSize;
	RtAudio::StreamOptions options;
	options.flags = RTAUDIO_MINIMIZE_LATENCY;

	try
	{
		m_Active = true;
		m_DAC->openStream(&params, NULL, RTAUDIO_SINT16, 44100, &buffer_frames, &mix, (void*)this);
		m_DAC->startStream();
		m_Initialized = true;
	}
	catch (RtError& e)
	{
		Message("Audio", "Fatal error: %s", e.getMessage().c_str());
	}
	return true;
}

RtAudioDevice::~RtAudioDevice()
{
	CloseStream();
	delete m_DAC;
}

void RtAudioDevice::CloseStream()
{
	try
	{
		if (m_DAC->isStreamOpen())
		{
			m_DAC->closeStream();
			m_DAC->abortStream();
		}
	}
	catch (RtError& e)
	{
		//Message("Audio", "Fatal error trying to stop stream: %s", e.getMessage().c_str());
	}
}

int RtAudioDevice::RegisterSound(const char* _Filename)
{
	if (!m_Initialized)
		Initialize();

	Resource::Sound* snd = GetKernel()->GetResourceManager()->Acquire<Resource::Sound>(_Filename);
	for(int i = 0; i < MAX_REGISTERED_SOUNDS; i++)
	{
		if (m_SoundBank[i] == snd)
		{
			Message("Audio", "Trying to register sound '%s' more than once.", _Filename);
			return i;
		}
	}

	for(int i = 0; i < MAX_REGISTERED_SOUNDS; i++)
	{
		if (m_SoundBank[i] == NULL)
		{
			m_SoundBank[i] = snd;
			return i;
		}
	}
	return -1;
}

int RtAudioDevice::RegisterSound(Resource::Sound* _Sound)
{
	if (!m_Initialized)
		Initialize();

	for(int i = 0; i < MAX_REGISTERED_SOUNDS; i++)
	{
		if (m_SoundBank[i] == _Sound)
		{
			Message("Audio", "Trying to register sound '%s' more than once.", _Sound->GetSource());
			return i;
		}
	}

	_Sound->_AddRef();
	for(int i = 0; i < MAX_REGISTERED_SOUNDS; i++)
	{
		if (m_SoundBank[i] == NULL)
		{
			m_SoundBank[i] = _Sound;
			return i;
		}
	}
	return -1;
}

int RtAudioDevice::GetSoundID(const Resource::Sound* _Sound)
{
	if (!m_Initialized)
		Initialize();

	for(int i = 0; i < MAX_REGISTERED_SOUNDS; i++)
	{
		if (m_SoundBank[i] == _Sound)
		{
			return i;
		}
	}
	return -1;
}

void RtAudioDevice::UnregisterSound(int _Id)
{
	if (!m_Initialized)
		Initialize();

	if(m_SoundBank[_Id])
	{
		for(unsigned i = 0; i < m_MaxVoices; i++)
		{
			if (m_ActiveVoices[i].clip == m_SoundBank[_Id])
				m_ActiveVoices[i].clip = 0;
		}

		GetKernel()->GetResourceManager()->Release(m_SoundBank[_Id]);
		m_SoundBank[_Id] = 0;
	}
}

void RtAudioDevice::Play(int _SoundID, bool _Loop)
{
	if (!m_Initialized)
		Initialize();

	if (m_SoundBank[_SoundID])
	{
		unsigned free_slot = -1;
		for(unsigned i = 0; i < m_MaxVoices; i++)
		{
			// Restart playback or resume paused sound
			if (m_ActiveVoices[i].clip == m_SoundBank[_SoundID])
			{
				if (m_ActiveVoices[i].active)
					m_ActiveVoices[i].current_frame = 0;
				m_ActiveVoices[i].active = true;
				return;
			}
			// id of free slot
			else if (m_ActiveVoices[i].clip == 0)
				free_slot = i;
		}

		// No sound with it was paused, and there is a voice available
		if (free_slot != -1)
		{
			m_ActiveVoices[free_slot].clip = m_SoundBank[_SoundID];
			m_ActiveVoices[free_slot].current_frame = 0;
			m_ActiveVoices[free_slot].active = true;
			m_ActiveVoices[free_slot].loop = _Loop;
			return;
		}
	}
}

void RtAudioDevice::Stop(int _SoundID)
{
	if (!m_Initialized)
		Initialize();

	if (m_SoundBank[_SoundID])
	{
		for(unsigned i = 0; i < m_MaxVoices; i++)
		{
			if (m_ActiveVoices[i].clip == m_SoundBank[_SoundID])
			{
				m_ActiveVoices[i].clip = 0;
				m_ActiveVoices[i].current_frame = 0;
				m_ActiveVoices[i].active = false;
				return;
			}
		}
	}
}

void RtAudioDevice::StopAll()
{
	if (!m_Initialized)
		Initialize();

	for(unsigned i = 0; i < m_MaxVoices; i++)
	{
		if (m_ActiveVoices[i].clip)
		{
			m_ActiveVoices[i].clip = 0;
			m_ActiveVoices[i].current_frame = 0;
			m_ActiveVoices[i].active = false;
			return;
		}
	}
}

void RtAudioDevice::Pause(int _SoundID)
{
	if (!m_Initialized)
		Initialize();

	if (m_SoundBank[_SoundID])
	{
		for(unsigned i = 0; i < m_MaxVoices; i++)
		{
			if (m_ActiveVoices[i].clip == m_SoundBank[_SoundID])
			{
				m_ActiveVoices[i].active = false;
				return;
			}
		}
	}
}

void RtAudioDevice::PauseAll()
{
	if (!m_Initialized)
		Initialize();

	for(unsigned i = 0; i < m_MaxVoices; i++)
	{
		if (m_ActiveVoices[i].clip)
		{
			m_ActiveVoices[i].active = false;
			return;
		}
	}
}

void RtAudioDevice::DumpInfo()
{
	if (!m_Initialized)
		Initialize();

	/* Enumerate audio devices */

	unsigned num_devices = m_DAC->getDeviceCount();
	Message("Audio", "Available output devices: %d", num_devices);

	RtAudio::DeviceInfo info;
	for(int i = 0; i < num_devices; i++)
	{
		info = m_DAC->getDeviceInfo(i);
		if (info.probed)
		{
			Message("Audio", " | %d%s %s (cout: %d; cin: %d)", i, info.isDefaultOutput ? " >" : ".", info.name.c_str()
				   , info.outputChannels, info.inputChannels);
		}
	}
}