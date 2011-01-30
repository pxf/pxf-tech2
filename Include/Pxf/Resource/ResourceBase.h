#ifndef _PXF_RESOURCE_RESOURCEBASE_H_
#define _PXF_RESOURCE_RESOURCEBASE_H_

#include <Pxf/Base/Types.h>
#include <Pxf/Util/Noncopyable.h>
#include <Pxf/Util/String.h>
#include <Pxf/Resource/Chunk.h>

namespace Pxf {
	class Kernel;
namespace Resource {
	class ResourceManager;
	class ResourceLoader;
	class ResourceBase : public Util::Noncopyable
	{
	protected:
		Kernel* m_Kernel;
		Chunk* m_Chunk;
		int m_References;
		ResourceLoader* m_Loader;
		virtual bool Build() pure;
		friend class ResourceManager;
	public:
		ResourceBase(Kernel* _Kernel, Chunk* _Chunk, ResourceLoader* _Loader)
			: m_Kernel(_Kernel) 
			, m_Chunk(_Chunk)
			, m_Loader(_Loader)
			, m_References(0)
		{}
		virtual ~ResourceBase()
		{
			if (m_Chunk)
				delete m_Chunk;
		};

		virtual const bool IsReady() const pure;

		const char* GetSource() const
		{
			return m_Chunk->source.c_str();
		}

		void _AddRef()
		{
			m_References++;
		}

		void _DeRef()
		{
			m_References--;
		}

	};

} // Resource
} // Pxf

#endif //_PXF_RESOURCE_RESOURCEBASE_H_


