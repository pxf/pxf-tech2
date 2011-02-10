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
		Text(Kernel* _Kernel, Resource::Chunk* _Chunk, Resource::ResourceLoader* _Loader)
			: Resource::ResourceBase(_Kernel, _Chunk, _Loader)
		{
			Build();
		}
		
		virtual ~Text();
		
		char* Ptr() const
		{
			return (char*)m_Chunk->data;
		}

		unsigned Length() const
		{
			return m_Chunk->size;
		}
	
		const bool IsReady() const
		{
			return m_Chunk != 0;
		}
	};


	class TextLoader : public Resource::ResourceLoader
	{
	private:
		unsigned m_LogTag;
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