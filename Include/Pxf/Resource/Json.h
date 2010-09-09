#ifndef _PXF_RESOURCE_JSON_H_
#define _PXF_RESOURCE_JSON_H_

#include <Pxf/Resource/ResourceBase.h>

#include <Pxf/Util/Array.h>
#include <Pxf/Util/Map.h>

namespace Pxf {

namespace Resource {

	class Json : public ResourceBase
	{
	public:

		class Value
		{
			virtual const char* asString() = 0;
			virtual int asInt() = 0;
			virtual bool asBool() = 0;
			virtual Util::Array<Value*>* asArray() = 0;
			virtual Util::Map<Value*, Value*>* asMap() = 0;
		};

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

