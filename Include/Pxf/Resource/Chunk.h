#ifndef _PXF_RESOURCE_CHUNK_H_
#define _PXF_RESOURCE_CHUNK_H_

#include <Pxf/Util/Noncopyable.h>

namespace Pxf {
namespace Resource {

    class Chunk : public Util::Noncopyable
    {
    public:
        char* data;
        unsigned size;
        const char* source;
        bool is_static;
        
        Chunk()
            : data(0)
            , size(0)
            , source(0)
            , is_static(false) 
        {}
        
        ~Chunk()
        {
            if (!is_static)
            {
                if (data)
                    delete [] data;
                data = 0;
                size = 0;
            }
        };
    };
    
    Chunk* LoadFile(const char* _FilePath);

} // Resource
} // Pxf

#endif //__PXF_RESOURCE_CHUNK_H__


