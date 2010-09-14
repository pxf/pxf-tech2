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

#elif defined (__LINUX__) || defined (__linux__)

#include <unistd.h>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>

//void process_mem_usage(double& vm_usage, double& resident_set)
long int sstat_memoryusage()
{
   using std::ios_base;
   using std::ifstream;
   using std::string;

   double vm_usage     = 0.0;
   double resident_set = 0.0;
   long int ret_val = 0;

   // 'file' stat seems to give the most reliable results
   //
   ifstream stat_stream("/proc/self/stat",ios_base::in);

   // dummy vars for leading entries in stat that we don't care about
   //
   string pid, comm, state, ppid, pgrp, session, tty_nr;
   string tpgid, flags, minflt, cminflt, majflt, cmajflt;
   string utime, stime, cutime, cstime, priority, nice;
   string O, itrealvalue, starttime;

   // the two fields we want
   //
   unsigned long vsize;
   long rss;

   stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
               >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
               >> utime >> stime >> cutime >> cstime >> priority >> nice
               >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

   long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
   vm_usage     = vsize / 1024.0;
   //resident_set = rss * page_size_kb;

   ret_val = rss * page_size_kb;

   return ret_val;
}

#else


long int sstat_memoryusage()
{
	return 420*1024*1024;
}

#endif