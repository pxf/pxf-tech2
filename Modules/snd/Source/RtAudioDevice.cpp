#include <Pxf/Modules/snd/RtAudioDevice.h>
#include <Pxf/Kernel.h>
#include <Pxf/Base/Logger.h>
#include <Pxf/Math/Math.h>

#include <Pxf/Resource/Sound.h>
#include <Pxf/Resource/ResourceManager.h>

#include <RtAudio.h>

#include <zthread/Guard.h>
#include <zthread/FastMutex.h>

using namespace Pxf;
using namespace Pxf::Modules;

#define MAX_REGISTERED_SOUNDS 128

ZThread::FastMutex g_Lock;

// TODO: a lock should be used when using voices.
// TODO: change from Util::Array to a simpler structure for better performance.
int mix(void *_outbuff, void *_inbuff, unsigned int _num_frames,
		double _time, RtAudioStreamStatus _status, void *_device)
{
	g_Lock.acquire();

	RtAudioDevice* device = (RtAudioDevice*)_device;

	if (device->_IsClosed())
	{
		g_Lock.release();
		return 2;
	}

	if (_status == RTAUDIO_OUTPUT_UNDERFLOW)
	{
		device->_ShowMixerWarning("Warning: underflow, stuttering might occur...");
	}
	
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
	g_Lock.release();
	return device->IsActive() ? 0 : 2;
}

RtAudioDevice::RtAudioDevice(Pxf::Kernel* _Kernel)
	: Pxf::Audio::AudioDevice(_Kernel, "Rt Audio Device")
	, m_Active(false)
	, m_Channels(2)
	, m_BufferSize(1024)
	, m_MaxVoices(8)
	, m_Initialized(false)
	, m_Closed(false)
	, m_LogTag(0)
{
	m_LogTag = _Kernel->CreateTag("snd");
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
		m_Kernel->Log(m_LogTag | Logger::IS_INFORMATION, "No available output devices");
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
		m_Closed = false;
		m_DAC->openStream(&params, NULL, RTAUDIO_SINT16, 44100, &buffer_frames, &mix, (void*)this);
		m_DAC->startStream();
		m_Initialized = true;
	}
	catch (RtError& e)
	{
		m_Kernel->Log(m_LogTag | Logger::IS_CRITICAL, "Fatal error: %s", e.getMessage().c_str());
	}
	return true;
}

RtAudioDevice::~RtAudioDevice()
{
	StopAll();
	ZThread::Guard<ZThread::FastMutex> g(g_Lock);
	CloseStream();
	m_Closed = true;
	delete m_DAC;
}

void RtAudioDevice::CloseStream()
{
	try
	{
		if (m_DAC->isStreamOpen())
		{
			// HACK: Why both? An exception is thrown when calling abortStream
			// when the stream is closed.
			m_DAC->closeStream();
			m_DAC->abortStream();
			m_Closed = true;
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

	ZThread::Guard<ZThread::FastMutex> g(g_Lock);

	Resource::Sound* snd = GetKernel()->GetResourceManager()->Acquire<Resource::Sound>(_Filename);
	for(int i = 0; i < MAX_REGISTERED_SOUNDS; i++)
	{
		if (m_SoundBank[i] == snd)
		{
			m_Kernel->Log(m_LogTag | Logger::IS_INFORMATION, "Trying to register sound '%s' more than once.", _Filename);
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

	ZThread::Guard<ZThread::FastMutex> g(g_Lock);

	for(int i = 0; i < MAX_REGISTERED_SOUNDS; i++)
	{
		if (m_SoundBank[i] == _Sound)
		{
			m_Kernel->Log(m_LogTag | Logger::IS_INFORMATION, "Trying to register sound '%s' more than once.", _Sound->GetSource());
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

	ZThread::Guard<ZThread::FastMutex> g(g_Lock);

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

	ZThread::Guard<ZThread::FastMutex> g(g_Lock);

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

	ZThread::Guard<ZThread::FastMutex> g(g_Lock);

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

	ZThread::Guard<ZThread::FastMutex> g(g_Lock);

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

	ZThread::Guard<ZThread::FastMutex> g(g_Lock);

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

	ZThread::Guard<ZThread::FastMutex> g(g_Lock);

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

	ZThread::Guard<ZThread::FastMutex> g(g_Lock);

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

	ZThread::Guard<ZThread::FastMutex> g(g_Lock);

	/* Enumerate audio devices */

	unsigned num_devices = m_DAC->getDeviceCount();
	m_Kernel->Log(m_LogTag | Logger::IS_INFORMATION, "Available output devices: %d", num_devices);

	RtAudio::DeviceInfo info;
	for(int i = 0; i < num_devices; i++)
	{
		info = m_DAC->getDeviceInfo(i);
		if (info.probed)
		{
			m_Kernel->Log(m_LogTag | Logger::IS_INFORMATION, " | %d%s %s (cout: %d; cin: %d)", i, info.isDefaultOutput ? " >" : "."
						 ,info.name.c_str(), info.outputChannels, info.inputChannels);
		}
	}
}

void RtAudioDevice::_ShowMixerWarning(const char* _Msg)
{
	m_Kernel->Log(m_LogTag | Logger::IS_WARNING, _Msg);
}