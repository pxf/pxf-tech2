#ifndef _PXF_RESOURCE_JSON_H_
#define _PXF_RESOURCE_JSON_H_

#include <Pxf/Resource/ResourceBase.h>

namespace Pxf {
namespace Resource {

	class Json : public ResourceBase
	{
	public:
		Json(Chunk* _Chunk, ResourceLoader* _Loader)
            : ResourceBase(_Chunk, _Loader)
        {

		}

		virtual ~Json()
        {}

		virtual const bool IsReady() const
		{
			return false;
		}
	};

} // Resource
} // Pxf

#endif //_PXF_RESOURCE_JSON_H_

