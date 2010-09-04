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

		virtual void SetData(unsigned int _VertCount, unsigned int _TriCount,const float* _Vertices, const unsigned int* _Indices,const float* _Normals);
	};

	class CtmMeshLoader : public Resource::ResourceLoader
    {
    private:
		CTMcontext m_Context;

        bool Init();
    public:
        CtmMeshLoader(Pxf::Kernel* _Kernel);
        ~CtmMeshLoader();
        virtual Resource::Mesh* Load(const char* _FilePath);
		virtual Resource::Mesh* CreateFrom(const void* _DataPtr, unsigned _DataLen);
        virtual void Destroy(void* _Resource)
        {
            if (_Resource)
                delete (Resource::Mesh*)_Resource;
        }
    };

} // Graphics
} // Pxf

#endif //_PXF_RESOURCE_MODELLOADER_H_
