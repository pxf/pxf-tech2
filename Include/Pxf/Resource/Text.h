#ifndef _PXF_RESOURCE_TEXT_H
#define _PXF_RESOURCE_TEXT_H

#include <Pxf/Resource/ResourceBase.h>
#include <Pxf/Resource/ResourceLoader.h>

namespace Pxf {
namespace Resource
{
    class Text : public Resource::ResourceBase
    {
    protected:
        virtual bool Build();
    public:
        Text(Resource::Chunk* _Chunk, Resource::ResourceLoader* _Loader)
            : Resource::ResourceBase(_Chunk, _Loader)
        {
            Build();
        }
        
        virtual ~Text();
		/*
		void* GetDataPtr() const
		{
			return m_Chunk->data;
		}

		unsigned GetDataLen() const
		{
			return m_Chunk->size;
		}
		*/
		const bool IsReady() const
		{
			return m_Chunk != 0;
		}
    };


    class TextLoader : public Resource::ResourceLoader
    {
    private:
        bool Init(){ return true; }
    public:
        TextLoader(Pxf::Kernel* _Kernel);
        ~TextLoader();
        virtual Resource::Text* Load(const char* _FilePath);
		virtual Resource::Text* CreateFrom(const void* _DataPtr, unsigned _DataLen);
        virtual void Destroy(void* _Resource)
        {
            if (_Resource)
                delete (Resource::Text*)_Resource;
        }
    };
}
}

#endif // _PXF_RESOURCE_TEXT_H