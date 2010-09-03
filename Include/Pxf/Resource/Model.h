#ifndef _PXF_RESOURCE_MODEL_H_
#define _PXF_RESOURCE_MODEL_H_

#include <Pxf/Resource/ResourceBase.h>

namespace Pxf {
namespace Resource {

	class Model : public ResourceBase
	{
	protected:

	public:
		Model(Chunk* _Chunk, ResourceLoader* _Loader)
            : ResourceBase(_Chunk, _Loader)
        {}

		virtual ~Model()
        {}

		virtual const bool IsReady() const
		{
			return true;
		}
	};

} // Resource
} // Pxf

#endif //_PXF_RESOURCE_MODEL_H_

