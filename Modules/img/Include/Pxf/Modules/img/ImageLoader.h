#ifndef _PXF_RESOURCE_GENERICIMAGELOADER_H_
#define _PXF_RESOURCE_GENERICIMAGELOADER_H_

#include <Pxf/Kernel.h>
#include <Pxf/Resource/ResourceLoader.h>


namespace Pxf{
    namespace Resource
    { class Chunk; }
    
namespace Modules {
    class GenericImageLoader : public Resource::ResourceLoader
    {
    private:
        bool Init(){ return true; }
    public:
        GenericImageLoader(Pxf::Kernel* _Kernel);
        ~GenericImageLoader();
        virtual void* Load(Resource::Chunk* _Chunk);
    };

} // Graphics
} // Pxf

#endif //_PXF_RESOURCE_GENERICIMAGELOADER_H_
