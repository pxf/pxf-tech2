#ifndef _PXF_RESOURCE_JSON_H_
#define _PXF_RESOURCE_JSON_H_

#ifdef CONF_WITH_LIBRARY_JSONCPP

#include <json/json.h>

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

		Json(Kernel* _Kernel, Chunk* _Chunk, ResourceLoader* _Loader)
            : ResourceBase(_Kernel, _Chunk, _Loader)
        {}
		virtual ~Json()
        {}

		virtual ::Json::Value& GetRoot() = 0;

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


#endif // CONF_WITH_LIBRARY_JSONCPP
#endif //_PXF_RESOURCE_JSON_H_

