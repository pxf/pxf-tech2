#include <Pxf/Resource/Blob.h>
#include <Pxf/Base/Debug.h>

using namespace Pxf;

bool Resource::Blob::Build()
{
	return true;
}
Resource::Blob::~Blob()
{
}

Resource::BlobLoader::BlobLoader(Pxf::Kernel* _Kernel)
    : ResourceLoader(_Kernel, "Blob loader")
{
}

Resource::BlobLoader::~BlobLoader()
{
}

Resource::Blob* Resource::BlobLoader::Load(const char* _FilePath)
{
    Resource::Chunk* chunk = Resource::LoadFile(_FilePath);                   
    if (!chunk)
    {
        Pxf::Message("BlobLoader", "Unable to create chunk from file '%s'", _FilePath);
        return NULL;
    }
    return new Resource::Blob(m_Kernel, chunk, this);
}

Resource::Blob* Resource::BlobLoader::CreateFrom(const void* _DataPtr, unsigned _DataLen)
{
	Resource::Chunk* chunk = new Resource::Chunk();
	chunk->data = (void*) _DataPtr;
	chunk->size = _DataLen;
	chunk->is_static = true;
	return new Blob(m_Kernel, chunk, this);
}