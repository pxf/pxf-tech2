#include <Pxf/Resource/Text.h>
#include <Pxf/Base/Debug.h>

#include <Pxf/Base/Stream.h>

using namespace Pxf;

bool Resource::Text::Build()
{
	return true;
}
Resource::Text::~Text()
{
}

Resource::TextLoader::TextLoader(Pxf::Kernel* _Kernel)
    : ResourceLoader(_Kernel, "Text loader")
{
}

Resource::TextLoader::~TextLoader()
{
}

Resource::Text* Resource::TextLoader::Load(const char* _FilePath)
{
    Resource::Chunk* chunk = new Resource::Chunk();
	FileStream f;
	f.Open(_FilePath, "r");
	unsigned size = f.GetSize();
	void* data = new char[size];
	f.Read(data, size);
	f.Close();

	chunk->data = data;
	chunk->size = size;
	chunk->is_static = false;
	chunk->source = _FilePath;

    if (!chunk)
    {
        Pxf::Message("TextLoader", "Unable to create chunk from file '%s'", _FilePath);
        return NULL;
    }
    return new Resource::Text(chunk, this);
}

Resource::Text* Resource::TextLoader::CreateFrom(const void* _DataPtr, unsigned _DataLen)
{
	Resource::Chunk* chunk = new Resource::Chunk();
	chunk->data = (void*) _DataPtr;
	chunk->size = _DataLen;
	chunk->is_static = true;
	return new Text(chunk, this);
}