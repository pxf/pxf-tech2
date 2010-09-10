#ifndef _PXF_RESOURCE_MODELLOADER_H_
#define _PXF_RESOURCE_MODELLOADER_H_

#include <Pxf/Kernel.h>
#include <Pxf/Resource/ResourceLoader.h>
#include <Pxf/Resource/Mesh.h>
#include <openctm.h>

namespace Pxf{
namespace Resource
{
    class Chunk;
}
    
namespace Modules {

	class OpenCTMMesh : public Resource::Mesh
	{
	protected:
        virtual bool Build();
	public:
		OpenCTMMesh(Resource::Chunk* _Chunk, Resource::ResourceLoader* _Loader)
			: Resource::Mesh(_Chunk,_Loader)
		{ }
		
		virtual ~OpenCTMMesh() { }
	};

	class CtmMeshLoader : public Resource::MeshLoader
    {
    private:
		CTMcontext m_Context;

        bool Init();
    public:
        CtmMeshLoader(Pxf::Kernel* _Kernel);
        virtual ~CtmMeshLoader();
        virtual Resource::Mesh* Load(const char* _FilePath);
		virtual Resource::Mesh* CreateFrom(const void* _DataPtr, unsigned _DataLen);
    };

} // Graphics
} // Pxf

#endif //_PXF_RESOURCE_MODELLOADER_H_
