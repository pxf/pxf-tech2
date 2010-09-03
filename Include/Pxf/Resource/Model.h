#ifndef _PXF_RESOURCE_MODEL_H_
#define _PXF_RESOURCE_MODEL_H_

#include <Pxf/Resource/ResourceBase.h>

namespace Pxf {
namespace Resource {

	class Model : public ResourceBase
	{
	public:
		struct model_descriptor 
		{
			bool has_normals;
			int vertex_count;
			int triangle_count;

			float* vertices;
			float* normals;
			unsigned int* indices;
		};

	protected:
		model_descriptor m_ModelData;
	public:
		Model(Chunk* _Chunk, ResourceLoader* _Loader)
            : ResourceBase(_Chunk, _Loader)
        {
			m_ModelData.has_normals = false;
			m_ModelData.vertex_count = 0;
			m_ModelData.triangle_count = 0;
			m_ModelData.vertices = 0;
			m_ModelData.normals = 0;
			m_ModelData.indices= 0;
		}

		virtual ~Model()
        {}

		virtual const bool IsReady() const
		{
			return true; // m_Vertices && !(m_HasNormals ^ (m_Normals == 0)) && m_Indices;
		}
	};

} // Resource
} // Pxf

#endif //_PXF_RESOURCE_MODEL_H_

