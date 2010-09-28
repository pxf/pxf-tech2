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

	// does nothing? hmz
	class OpenCTMMesh : public Resource::Mesh
	{
	protected:
		unsigned m_LogTag;
		virtual bool Build();
	public:
		OpenCTMMesh(Kernel* _Kernel, Resource::Chunk* _Chunk, Resource::ResourceLoader* _Loader);
		
		virtual ~OpenCTMMesh() { }
	};

	class CtmMeshLoader : public Resource::MeshLoader
	{
	private:
		unsigned m_LogTag;
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
