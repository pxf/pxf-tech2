#ifndef _PXF_BASE_DEBUG_H_
#define _PXF_BASE_DEBUG_H_

#include <Pxf/Base/Config.h>

namespace Pxf
{
	/* Static Assert */
	#define PXF_STATIC_ASSERT(e, m) typedef \
	struct { int _PXF_STATIC_ASSERT_NAME_(__LINE__): ((e) ? 1 : -1); } \
	_PXF_STATIC_ASSERT_NAME_(__LINE__)[(e) ? 1 : -1]
	#define _PXF_STATIC_ASSERT_NAME_(line)	_PXF_STATIC_ASSERT_NAME2_(line)
	#define _PXF_STATIC_ASSERT_NAME2_(line)	static_assert_failed_at_line_##line
    
	/* Assert */
	#if defined(CONF_DEBUG) && defined(CONF_COMPILER_MSVC)
		#define PXF_ASSERT(_Expression, _Message, ...) do{\
			if(!Pxf::Assert(_Expression, __LINE__, __FILE__, _Message, ##__VA_ARGS__))\
			{__asm {int 3};}}while(0)

	#elif defined(CONF_DEBUG) && defined(CONF_COMPILER_GCC)
		#define PXF_ASSERT(_Expression, _Message, ...) Pxf::Assert(_Expression, __LINE__, __FILE__, _Message, ##__VA_ARGS__)
	#else
		#define PXF_ASSERT(_Expression, _Message, ...) do{}while(0)
	#endif
	bool Assert(bool _Expression, int _Line, const char* _File, const char* _Message, ...);
	
	/* Simple message reporting */
	void Message(const char* _SubSystem, const char* _Message, ...);

}

#endif //_PXF_BASE_DEBUG_H_

