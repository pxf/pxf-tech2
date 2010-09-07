#include <Pxf/Modules/snd/RtAudioDevice.h>
#include <RtAudio.h>

using namespace Pxf;
using namespace Pxf::Modules;

int mix(void *_outbuff, void *_inbuff, unsigned int _num_frames,
		double _time, RtAudioStreamStatus _status, void *_userdata)
{
	for(int i = 0; i < _num_frames; i++)
	{
		// Mix frames of playing audio
	}
	return 0;
}

bool RtAudioDevice::Init()
{
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

	unsigned int buffer_frames = 256;

	try
	{
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
		m_DAC->stopStream();
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
	return -1;
}

int RtAudioDevice::GetSoundID(const Resource::Sound* _Sound)
{
	return -2;
}

void RtAudioDevice::UnregisterSound(int _Id)
{

}

void RtAudioDevice::Play(unsigned int _SoundID)
{

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
