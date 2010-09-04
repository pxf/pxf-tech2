#ifndef _PXF_RESOURCE_MESH_H_
#define _PXF_RESOURCE_MESH_H_

#include <Pxf/Resource/ResourceBase.h>

namespace Pxf {
namespace Resource {

	class Mesh : public ResourceBase
	{
	public:
		struct mesh_descriptor 
		{
			bool has_normals;
			int vertex_count;
			int triangle_count;

			const float* vertices;
			const float* normals;
			const unsigned int* indices;
		};

	protected:
		mesh_descriptor m_MeshData;
	public:
		Mesh(Chunk* _Chunk, ResourceLoader* _Loader)
            : ResourceBase(_Chunk, _Loader)
        {
			m_MeshData.has_normals = false;
			m_MeshData.vertex_count = 0;
			m_MeshData.triangle_count = 0;
			m_MeshData.vertices = 0;
			m_MeshData.normals = 0;
			m_MeshData.indices= 0;
		}

		virtual ~Mesh()
        {}

		virtual const bool IsReady() const
		{
			return m_MeshData.vertices && !(m_MeshData.has_normals ^ (m_MeshData.normals == 0)) && m_MeshData.indices;
		}
	};

} // Resource
} // Pxf

#endif //_PXF_RESOURCE_MESH_H_

