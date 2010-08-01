#ifndef _PXF_RESOURCE_IMAGELOADER_H_
#define _PXF_RESOURCE_IMAGELOADER_H_

#include <Pxf/Kernel.h>
#include <Pxf/Resource/ResourceLoader.h>
#include <Pxf/Resource/Image.h>


namespace Pxf{
namespace Resource
{
    class Chunk;
}
    
namespace Modules {

    class SOILImage : protected Resource::Image
    {
    protected:
        virtual bool Build();
    public:
        SOILImage(Resource::Chunk* _Chunk)
            : Resource::Image(_Chunk)
        {
            Build();
        }
        
        virtual ~SOILImage();
    };

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

#endif //_PXF_RESOURCE_IMAGELOADER_H_
