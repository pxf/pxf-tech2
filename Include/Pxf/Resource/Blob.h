#ifndef _PXF_RESOURCE_BLOB_H
#define _PXF_RESOURCE_BLOB_H

#include <Pxf/Resource/ResourceBase.h>
#include <Pxf/Resource/ResourceLoader.h>

namespace Pxf {
namespace Resource
{
    class Blob : public Resource::ResourceBase
    {
    protected:
        virtual bool Build();
    public:
        Blob(Resource::Chunk* _Chunk, Resource::ResourceLoader* _Loader)
            : Resource::ResourceBase(_Chunk, _Loader)
        {
            Build();
        }
        
        virtual ~Blob();

		void* GetDataPtr() const
		{
			return m_Chunk->data;
		}

		unsigned GetDataLen() const
		{
			return m_Chunk->size;
		}

		const bool IsReady() const
		{
			return m_Chunk != 0;
		}
    };

    class BlobLoader : public Resource::ResourceLoader
    {
    private:
        bool Init(){ return true; }
    public:
        BlobLoader(Pxf::Kernel* _Kernel);
        ~BlobLoader();
        virtual Resource::Blob* Load(const char* _FilePath);
		virtual Resource::Blob* CreateFrom(const void* _DataPtr, unsigned _DataLen);
        virtual void Destroy(void* _Resource)
        {
            if (_Resource)
                delete (Resource::Blob*)_Resource;
        }
    };
}
}

#endif // _PXF_RESOURCE_BLOB_H