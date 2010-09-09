#ifndef _PXF_RESOURCE_JSON_H_
#define _PXF_RESOURCE_JSON_H_

#include <Pxf/Resource/ResourceBase.h>

#include <Pxf/Util/Array.h>
#include <Pxf/Util/Map.h>
#include <Pxf/Util/String.h>

namespace Pxf {

namespace Resource {

	class Json : public ResourceBase
	{
	public:

		class Value
		{
			virtual int asInt() = 0;
			virtual bool asBool() = 0;
			virtual Util::String asString() = 0;
			virtual Util::Array<Value*> asArray() = 0;
			virtual Util::Map<Value*, Value*> asMap() = 0;
		};

		Json(Chunk* _Chunk, ResourceLoader* _Loader)
            : ResourceBase(_Chunk, _Loader)
        {}
		virtual ~Json()
        {}

		virtual Value* Get(const char* _String, const char* _DefaultValue) = 0;
		virtual Value* Get(const String _String, const String _DefaultValue) = 0;
		virtual Value* Get(int _Value, int _DefaultValue) = 0;
		virtual Value* Get(bool _Value, bool _DefaultValue) = 0;

		virtual const bool IsReady() const
		{
			return false;
		}
	};

} // Resource
} // Pxf

#endif //_PXF_RESOURCE_JSON_H_

