#ifndef _PXF_UTIL_NONCOPYABLE_H_
#define _PXF_UTIL_NONCOPYABLE_H_

namespace Pxf {
namespace Util {

// protect from unintended ADL
namespace Noncopyable_
{
	class Noncopyable
	{
	protected:
		Noncopyable(){};
		~Noncopyable(){};
	private:
		Noncopyable(const Noncopyable&);
		const Noncopyable& operator=(const Noncopyable&);
	};
}
typedef Noncopyable_::Noncopyable Noncopyable;

} // Util
} // Pxf

#endif //_PXF_UTIL_NONCOPYABLE_H_


