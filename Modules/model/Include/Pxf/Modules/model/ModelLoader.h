#ifndef _PXF_RESOURCE_MODELLOADER_H_
#define _PXF_RESOURCE_MODELLOADER_H_

#include <Pxf/Kernel.h>
#include <Pxf/Resource/ResourceLoader.h>
#include <Pxf/Resource/Model.h>


namespace Pxf{
namespace Resource
{
    class Chunk;
}
    
namespace Modules {

	class OpenCTMModel : public Resource::Model
	{
	public:
		OpenCTMModel(Resource::Chunk* _Chunk, Resource::ResourceLoader* _Loader)
			: Resource::Model(_Chunk,_Loader)
		{ }
		
		virtual ~OpenCTMModel() { }
	};

	class GenericModelLoader : public Resource::ResourceLoader
    {
    private:
        bool Init(){ return true; }
    public:
        GenericModelLoader(Pxf::Kernel* _Kernel);
        ~GenericModelLoader();
        virtual Resource::Model* Load(const char* _FilePath);
		virtual Resource::Model* CreateFrom(const void* _DataPtr, unsigned _DataLen);
        virtual void Destroy(void* _Resource)
        {
            if (_Resource)
                delete (Resource::Model*)_Resource;
        }
    };

} // Graphics
} // Pxf

#endif //_PXF_RESOURCE_MODELLOADER_H_
