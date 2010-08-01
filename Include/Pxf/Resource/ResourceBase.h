#ifndef _PXF_RESOURCE_RESOURCEBASE_H_
#define _PXF_RESOURCE_RESOURCEBASE_H_

#include <Pxf/Base/Types.h>
#include <Pxf/Util/Noncopyable.h>
#include <Pxf/Util/String.h>
#include <Pxf/Resource/Chunk.h>

namespace Pxf {
namespace Resource {
    class ResourceBase : public Util::Noncopyable
    {
    protected:
        Chunk* m_Chunk;
        unsigned m_References;
        virtual bool Build() pure;

    public:
        ResourceBase(Chunk* _Chunk)
            : m_Chunk(_Chunk)
            , m_References(0)
        {}
        virtual ~ResourceBase() {};

        virtual const bool IsReady() const pure;

        const char* GetSource() const
        {
            return m_Chunk->source;
        }
    };

} // Resource
} // Pxf

#endif //_PXF_RESOURCE_RESOURCEBASE_H_


