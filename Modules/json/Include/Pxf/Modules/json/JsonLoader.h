#ifndef _PXF_RESOURCE_JSONLOADER_H_
#define _PXF_RESOURCE_JSONLOADER_H_

#include <Pxf/Kernel.h>
#include <Pxf/Resource/ResourceLoader.h>
#include <Pxf/Resource/Json.h>

namespace Pxf{
namespace Resource
{
    class Chunk;
}
    
namespace Modules {

	class JsonCpp : public Resource::Json
	{
	protected:
		::Json::Reader m_Reader;
		::Json::Value m_Root;
        virtual bool Build();
	public:
		JsonCpp(Kernel* _Kernel, Resource::Chunk* _Chunk, Resource::ResourceLoader* _Loader)
			: Resource::Json(_Kernel, _Chunk,_Loader)
		{
			Build();
		}

		virtual ~JsonCpp();
		
		virtual ::Json::Value& GetRoot()
		{
			return m_Root;
		}
	};

	class JsonCppLoader : public Resource::JsonLoader
    {
    private:
        bool Init();
    public:
        JsonCppLoader(Pxf::Kernel* _Kernel);
        virtual ~JsonCppLoader();
        virtual Resource::Json* Load(const char* _FilePath);
		virtual Resource::Json* CreateFrom(const void* _DataPtr, unsigned _DataLen);
    };

} // Graphics
} // Pxf

#endif //_PXF_RESOURCE_JSONLOADER_H_
