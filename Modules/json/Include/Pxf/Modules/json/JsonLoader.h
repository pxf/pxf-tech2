#ifndef _PXF_RESOURCE_JSONLOADER_H_
#define _PXF_RESOURCE_JSONLOADER_H_

#include <Pxf/Kernel.h>
#include <Pxf/Resource/ResourceLoader.h>
#include <Pxf/Resource/Json.h>
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
			virtual int asInt();
			virtual bool asBool();
			virtual Util::String asString();
			virtual Util::Array<Value*> asArray();
			virtual Util::Map<Value*, Value*> asMap();
		};
	protected:
        virtual bool Build();
	public:
		JsonCpp(Resource::Chunk* _Chunk, Resource::ResourceLoader* _Loader)
			: Resource::Json(_Chunk,_Loader)
		{ }
		
		virtual Value* Get(const char* _String, const char* _DefaultValue);
		virtual Value* Get(const Util::String _String, const Util::String _DefaultValue);
		virtual Value* Get(int _Value, int _DefaultValue);
		virtual Value* Get(bool _Value, bool _DefaultValue);


		virtual ~JsonCpp() { }
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
