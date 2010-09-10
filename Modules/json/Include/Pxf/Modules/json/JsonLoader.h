#ifndef _PXF_RESOURCE_JSONLOADER_H_
#define _PXF_RESOURCE_JSONLOADER_H_

#include <Pxf/Kernel.h>
#include <Pxf/Resource/ResourceLoader.h>
#include <Pxf/Resource/Json.h>

namespace Json
{
	class Reader;
	class Value;
}

namespace Pxf{
namespace Resource
{
    class Chunk;
}
    
namespace Modules {

	class JsonCpp : public Resource::Json
	{
	public:
		class JsonCppValue : public Resource::Json::Value
		{
		public:
			virtual int asInt();
			virtual bool asBool();
			virtual Util::String asString();
			virtual Util::Array<Value*> asArray();
			virtual Util::Map<Value*, Value*> asMap();
		};
	protected:
		::Json::Reader* m_Reader;
		::Json::Value* m_Root;
        virtual bool Build();
	public:
		JsonCpp(Kernel* _Kernel, Resource::Chunk* _Chunk, Resource::ResourceLoader* _Loader)
			: Resource::Json(_Kernel, _Chunk,_Loader)
			, m_Reader(0)
			, m_Root(0)
		{
			Build();
		}

		virtual ~JsonCpp();
		
		virtual Value* Get(const char* _String, const char* _DefaultValue);
		virtual Value* Get(const char* _String, int _DefaultValue);
		virtual Value* Get(const char* _String, bool _DefaultValue);
		virtual Value* Get(int _Value, int _DefaultValue);
		virtual Value* Get(bool _Value, bool _DefaultValue);
	};

	class JsonCppLoader : public Resource::JsonLoader
    {
    private:
        bool Init();
    public:
        JsonCppLoader(Pxf::Kernel* _Kernel);
        virtual ~JsonCppLoader();
        virtual Resource::Json* Load(const char* _FilePath);
		virtual Resource::Json* CreateFrom(const void* _DataPtr, unsigned _DataLen);
    };

} // Graphics
} // Pxf

#endif //_PXF_RESOURCE_JSONLOADER_H_
