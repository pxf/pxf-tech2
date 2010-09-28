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
			file.Close();
			return NULL;
		}

		Resource::Chunk* chunk = new Resource::Chunk();

		int read = 0;
		if((read = file.Read(data, size)) > 0)
		{
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
		return NULL;
	}
	return NULL;
}