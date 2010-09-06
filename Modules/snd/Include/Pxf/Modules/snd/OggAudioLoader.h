#ifndef _PXF_RESOURCE_IMAGELOADER_H_
#define _PXF_RESOURCE_IMAGELOADER_H_

#include <Pxf/Kernel.h>
#include <Pxf/Resource/ResourceLoader.h>
#include <Pxf/Resource/Sound.h>


namespace Pxf{
namespace Resource
{
    class Chunk;
}
    
namespace Modules {

    class OggSound : public Resource::Sound
    {
    protected:
        virtual bool Build();
    public:
        OggSound(Resource::Chunk* _Chunk, Resource::ResourceLoader* _Loader)
            : Resource::Sound(_Chunk, _Loader)
        {
            Build();
        }
        
        virtual ~OggSound();
    };

    class OggAudioLoader : public Resource::ResourceLoader
    {
    private:
        bool Init(){ return true; }
    public:
        OggAudioLoader(Pxf::Kernel* _Kernel);
        ~OggAudioLoader();
        virtual Resource::Sound* Load(const char* _FilePath);
		virtual Resource::Sound* CreateFrom(const void* _DataPtr, unsigned _DataLen);
        virtual void Destroy(void* _Resource)
        {
            if (_Resource)
                delete (Resource::Sound*)_Resource;
        }
    };

} // Graphics
} // Pxf

#endif //_PXF_RESOURCE_IMAGELOADER_H_
