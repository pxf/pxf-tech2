#ifndef _PXF_RESOURCE_JSON_H_
#define _PXF_RESOURCE_JSON_H_

#include <Pxf/Resource/ResourceBase.h>
#include <Pxf/Resource/ResourceLoader.h>

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
		public:
			virtual int asInt() = 0;
			virtual bool asBool() = 0;
			virtual Util::String asString() = 0;
			virtual Util::Array<Value*> asArray() = 0;
			virtual Util::Map<Value*, Value*> asMap() = 0;
		};

		Json(Kernel* _Kernel, Chunk* _Chunk, ResourceLoader* _Loader)
            : ResourceBase(_Kernel, _Chunk, _Loader)
        {}
		virtual ~Json()
        {}

		virtual Value* Get(const char* _String, const char* _DefaultValue) = 0;
		virtual Value* Get(const char* _String, int _DefaultValue) = 0;
		virtual Value* Get(const char* _String, bool _DefaultValue) = 0;
		virtual Value* Get(int _Value, int _DefaultValue) = 0;
		virtual Value* Get(bool _Value, bool _DefaultValue) = 0;

		virtual const bool IsReady() const
		{
			return false;
		}
	};

	class JsonLoader : public Resource::ResourceLoader
	{
	private:
		bool Init(){ return true; }
	public:
		JsonLoader(Pxf::Kernel* _Kernel, const char* _Identifier)
			: ResourceLoader(_Kernel, _Identifier)
		{}
		virtual ~JsonLoader() {};
		virtual Resource::Json* Load(const char* _FilePath) = 0;
		virtual Resource::Json* CreateFrom(const void* _DataPtr, unsigned _DataLen) = 0;
		virtual void Destroy(void* _Resource)
		{
			if (_Resource)
				delete (Resource::Json*)_Resource;
		}
	};

} // Resource
} // Pxf

#endif //_PXF_RESOURCE_JSON_H_

