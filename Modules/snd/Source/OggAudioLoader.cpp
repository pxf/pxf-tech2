#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Modules/snd/OggAudioLoader.h>
#include <Pxf/Resource/Chunk.h>

#include <SOIL.h>

using namespace Pxf;

bool Modules::OggSound::Build()
{

    return true;
}
Modules::OggSound::~OggSound()
{

}

Modules::OggAudioLoader::OggAudioLoader(Pxf::Kernel* _Kernel)
    : ResourceLoader(_Kernel, "Ogg Audio Loader")
{
}

Modules::OggAudioLoader::~OggAudioLoader()
{
}

Resource::Sound* Modules::OggAudioLoader::Load(const char* _FilePath)
{
    Resource::Chunk* chunk = Resource::LoadFile(_FilePath);                   
    if (!chunk)
    {
        Message("AudioLoader", "Unable to create chunk from file '%s'", _FilePath);
        return NULL;
    }
    return new OggSound(chunk, this);
}

Resource::Sound* Modules::OggAudioLoader::CreateFrom(const void* _DataPtr, unsigned _DataLen)
{
	Resource::Chunk* chunk = new Resource::Chunk();
	chunk->data = (void*) _DataPtr;
	chunk->size = _DataLen;
	chunk->is_static = true;
	return new OggSound(chunk, this);
}