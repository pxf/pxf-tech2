#ifndef _PXF_RESOURCE_RESOURCEBASE_H_
#define _PXF_RESOURCE_RESOURCEBASE_H_

#include <Pxf/Base/Types.h>
#include <Pxf/Util/Noncopyable.h>
#include <Pxf/Util/String.h>

namespace Pxf {
namespace Resource {

	class Chunk;

	class ResourceBase : public Util::Noncopyable
	{
	protected:
		Chunk* m_Chunk;
		const char* m_Source;
		unsigned m_References;
		virtual bool Build() pure;

	public:
		ResourceBase(Chunk* _Chunk, const char* _Source)
			: m_Chunk(_Chunk)
			, m_References(0)
			, m_Source(_Source)
		{}
		virtual ~ResourceBase()
		{};

		virtual const bool IsReady() const pure;

		const char* GetSource() const
		{
			return m_Source;
		}
	};

} // Resource
} // Pxf

#endif //_PXF_RESOURCE_RESOURCEBASE_H_


