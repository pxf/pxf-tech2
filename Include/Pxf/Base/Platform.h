#ifndef _PXF_BASE_PLATFORM_H_
#define _PXF_BASE_PLATFORM_H_

#include <Pxf/Base/Types.h>
#include <Pxf/Base/Config.h>

namespace Pxf
{
	class Platform
	{
	private:
		static bool s_TimeInitialized;
		static int64 m_Frequency;
		static int64 m_Timestamp;
		static void Initialize();
	public:
		Platform()
		{
			if (!s_TimeInitialized)
			{
				Initialize();
				s_TimeInitialized = true;
			}
		}
		
		static int64 GetTime();
		static int32 GetNumberOfProcessors();

		/* Threading */
		void ThreadSleep(int32 _ms);
		void* ThreadCreate(void (*func)(void *), void *userdata);
		void ThreadWait(void* thread);
		void ThreadYield(void* thread);

		/* Mutex */
		typedef void* Lock;
		Lock LockCreate();
		void LockDestroy(Lock _Lock);
		bool LockTry(Lock _Lock);
		void LockWait(Lock _Lock);
		void LockRelease(Lock _Lock);

	};
}

#endif //_PXF_BASE_PLATFORM_H_

