#include <Pxf/Modules/snd/RtAudioDevice.h>
#include <RtAudio.h>
#include <Pxf/Math/Math.h>

#include <Pxf/Resource/Sound.h>

using namespace Pxf;
using namespace Pxf::Modules;

#define MAX_REGISTERED_SOUNDS 16
#define MAX_PLAYING_SOUNDS 5

const Resource::Sound* g_Clip;
unsigned pos = 0;

int mix(void *_outbuff, void *_inbuff, unsigned int _num_frames,
		double _time, RtAudioStreamStatus _status, void *_device)
{
	if (_status == RTAUDIO_OUTPUT_UNDERFLOW)
	{
		Message("Mixer", "Warning: underflow, stuttering might occur...");
	}
	
	RtAudioDevice* device = (RtAudioDevice*)_device;
	short* out = (short*)_outbuff;
	Util::Array<RtAudioDevice::SoundEntry*>* soundbank = device->GetSoundEntries();
	
	RtAudioDevice::SoundEntry* entry;
	for(unsigned int i = 0; i < _num_frames*2; i += 2)
	{
		out[i] = 0;
		out[i+1] = 0;
		
		for (int j = 0; j < MAX_REGISTERED_SOUNDS; j++)
		{
			entry = soundbank->at(j);
			if (entry && entry->active)
			{
				if (entry->current_frame >= entry->clip->DataLen())
				{
					if (entry->loop)
						entry->current_frame = 0;
					else
					{
						entry->active = false;
						continue;
					}
				}
				out[i+0] += entry->clip->DataPtr()[entry->current_frame + 0];
				out[i+1] += entry->clip->DataPtr()[entry->current_frame + 1];
				entry->current_frame += 2;
			}
			else
			{
				out[i+0] += 0;
				out[i+1] += 0;
			}
		
		}
	}
	return device->IsActive() ? 0 : 2;
}

bool RtAudioDevice::Init()
{
	m_SoundEntries.resize(MAX_REGISTERED_SOUNDS);

	m_DAC = new RtAudio();

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

	unsigned int buffer_frames = 1024;
	RtAudio::StreamOptions options;
	options.flags = RTAUDIO_MINIMIZE_LATENCY;

	try
	{
		m_Active = true;
		m_DAC->openStream(&params, NULL, RTAUDIO_SINT16, 44100, &buffer_frames, &mix, (void*)this);
		m_DAC->startStream();
	}
	catch (RtError& e)
	{
		Message("Audio", "Fatal error: %s", e.getMessage());
	}
	return true;
}

RtAudioDevice::~RtAudioDevice()
{
	try
	{
		m_DAC->abortStream();
	}
	catch (RtError& e)
	{
		Message("Audio", "Fatal error trying to stop stream: %s", e.getMessage());
	}

	if (m_DAC->isStreamOpen())
	{
		m_DAC->closeStream();
	}
}

int RtAudioDevice::RegisterSound(const Resource::Sound* _Sound)
{
	for(int i = 0; i < MAX_REGISTERED_SOUNDS; i++)
	{
		if (m_SoundEntries[i] == NULL)
		{
			RtAudioDevice::SoundEntry* entry
				= new RtAudioDevice::SoundEntry(_Sound, 0, false, false);
			m_SoundEntries[i] = entry;
			return i;
		}
	}
	return -1;
}

int RtAudioDevice::GetSoundID(const Resource::Sound* _Sound)
{
	return -2;
}

void RtAudioDevice::UnregisterSound(int _Id)
{

}

void RtAudioDevice::Play(unsigned int _SoundID, bool _Loop)
{
	if (m_SoundEntries[_SoundID])
	{
		m_SoundEntries[_SoundID]->active = true;
		m_SoundEntries[_SoundID]->loop = _Loop;
		m_SoundEntries[_SoundID]->current_frame = 0;
	}
}

void RtAudioDevice::Stop(unsigned int _SoundID)
{

}

void RtAudioDevice::StopAll()
{

}

void RtAudioDevice::Pause(unsigned int _SoundID)
{

}

void RtAudioDevice::PauseAll()
{

}
