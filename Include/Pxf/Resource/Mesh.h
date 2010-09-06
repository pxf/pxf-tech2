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
			mesh_descriptor() { }
			mesh_descriptor(const mesh_descriptor &cpy) 
			{
				has_normals = cpy.has_normals;
				vertex_count = cpy.vertex_count;
				triangle_count = cpy.triangle_count;
				vertices = cpy.vertices;
				normals = cpy.normals;
				indices = cpy.indices;
			}

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

		mesh_descriptor* GetData() { return &m_MeshData; }
		void SetData(const mesh_descriptor &_Data) { m_MeshData = _Data; }
	};

} // Resource
} // Pxf

#endif //_PXF_RESOURCE_MESH_H_

