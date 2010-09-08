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
        virtual bool Build();
	public:
		JsonCpp(Resource::Chunk* _Chunk, Resource::ResourceLoader* _Loader)
			: Resource::Json(_Chunk,_Loader)
		{ }
		
		virtual ~JsonCpp() { }
	};

	class JsonLoader : public Resource::ResourceLoader
    {
    private:
        bool Init();
    public:
        JsonLoader(Pxf::Kernel* _Kernel);
        ~JsonLoader();
        virtual Resource::Json* Load(const char* _FilePath);
		virtual Resource::Json* CreateFrom(const void* _DataPtr, unsigned _DataLen);
        virtual void Destroy(void* _Resource)
        {
            if (_Resource)
                delete (Resource::Json*)_Resource;
        }
    };

} // Graphics
} // Pxf

#endif //_PXF_RESOURCE_JSONLOADER_H_
