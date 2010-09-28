#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Modules/snd/OggAudioLoader.h>
#include <Pxf/Resource/Chunk.h>
#include <Pxf/Kernel.h>
#include <Pxf/Base/Logger.h>

#include <stb_vorbis.h>

#include <cstdlib>
#include <cstdio>

using namespace Pxf;

bool Modules::OggSound::Build()
{
	m_SoundDataLen = stb_vorbis_decode_memory((unsigned char*)m_Chunk->data, m_Chunk->size, &m_Channels, &m_SoundData);
	m_SoundDataLen *= m_Channels;
	return true;
}
Modules::OggSound::~OggSound()
{
	if (m_SoundData)
		free(m_SoundData);
}

Modules::OggAudioLoader::OggAudioLoader(Pxf::Kernel* _Kernel)
	: SoundLoader(_Kernel, "Ogg Audio Loader")
	, m_LogTag(0)
{
	m_LogTag = m_Kernel->CreateTag("res");
}

Modules::OggAudioLoader::~OggAudioLoader()
{
}

Resource::Sound* Modules::OggAudioLoader::Load(const char* _FilePath)
{
	Resource::Chunk* chunk = Resource::LoadFile(_FilePath);				   
	if (!chunk)
	{
		m_Kernel->Log(m_LogTag | Logger::IS_CRITICAL, "Unable to create chunk from file '%s'", _FilePath);
		return NULL;
	}
	return new OggSound(m_Kernel, chunk, this);
}

Resource::Sound* Modules::OggAudioLoader::CreateFrom(const void* _DataPtr, unsigned _DataLen)
{
	Resource::Chunk* chunk = new Resource::Chunk();
	chunk->source = "unknown";
	chunk->data = (void*) _DataPtr;
	chunk->size = _DataLen;
	chunk->is_static = true;
	return new OggSound(m_Kernel, chunk, this);
}