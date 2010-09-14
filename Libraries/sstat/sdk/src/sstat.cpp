#include "sstat.h"

#if defined (WIN32) || defined (_WIN32) || defined(__CYGWIN32__) || defined (__MINGW32__)

#include <windows.h>
#include <psapi.h>

long int sstat_memoryusage()
{
	DWORD  prof_processid;
	HANDLE prof_hprocess;
	PROCESS_MEMORY_COUNTERS prof_pmc;
	long int    prof_memusage = 0;
	
	prof_processid = GetCurrentProcessId();
	prof_hprocess = OpenProcess(  PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, prof_processid );
	
    if (NULL == prof_hprocess)
      return -1;

    if ( GetProcessMemoryInfo( prof_hprocess, &prof_pmc, sizeof(prof_pmc)) )
    {
      prof_memusage = prof_pmc.WorkingSetSize;
    }

    CloseHandle( prof_hprocess );
    
	return prof_memusage;
}

#elif defined (MACOSX) || defined (__APPLE__)

#include <mach/mach.h>
#include <mach/mach_init.h>
#include <mach/task.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

long int sstat_memoryusage()
{
	task_basic_info         info;
	kern_return_t           rval = 0;
	mach_port_t             task = mach_task_self();
	mach_msg_type_number_t  tcnt = TASK_BASIC_INFO_COUNT;
	task_info_t             tptr = (task_info_t) &info;
	memset(&info, 0, sizeof(info));
	rval = task_info(task, TASK_BASIC_INFO, tptr, &tcnt);
	if (!(rval == KERN_SUCCESS))
		return 0;
	
	return info.resident_size;
}

#endif