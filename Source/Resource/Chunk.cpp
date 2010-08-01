#include <Pxf/Resource/Chunk.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Types.h>
#include <Pxf/Base/Stream.h>

using namespace Pxf;

Resource::Chunk* Resource::LoadFile(const char* _FilePath)
{
	FileStream file;
	if (file.OpenReadBinary(_FilePath))
	{
		uint size = file.GetSize();
		char* data = new char[size];

		if (!data)
		{
			Message("Chunk::ReadFile", "Could not create chunk, out of memory");
			file.Close();
			return NULL;
		}

		Resource::Chunk* chunk = new Resource::Chunk();

		int read = 0;
		if((read = file.Read(data, size)) > 0)
		{
			Message("Chunk::ReadFile", "Creating chunk (0x%x) from '%s' (read %d of %d bytes)", chunk, _FilePath, read, size);
			chunk->data = data;
			chunk->size = size;
            chunk->source = _FilePath;
            chunk->is_static = false;
			file.Close();
			return chunk;
		}

		file.Close();
		SafeDeleteArray(data);
		SafeDelete(chunk);
		Message("Chunk::ReadFile", "Failed to read from '%s'", _FilePath);
		return NULL;
	}
	
	Message("Chunk::ReadFile", "Failed to open '%s'", _FilePath);
	return NULL;
}