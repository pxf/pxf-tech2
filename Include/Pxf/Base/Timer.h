#ifndef _PXF_BASE_TIMER_H_
#define _PXF_BASE_TIMER_H_

#include <Pxf/Base/Types.h>

/*
	Basic timer with millisecond percision.

	TODO: Microseconds.
*/

namespace Pxf
{
	class Timer
	{
	private:
		uint64 m_Start;
		uint64 m_Stop;
		bool m_Stopped;
	public:
		Timer();
		void Start();
		void Stop();
		uint64 Interval();
	};
}

#endif //_PXF_BASE_TIMER_H_

