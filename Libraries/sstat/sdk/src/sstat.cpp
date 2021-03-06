#include "sstat.h"

#if defined (WIN32) || defined (_WIN32) || defined(__CYGWIN32__) || defined (__MINGW32__)

#include <windows.h>
#include <psapi.h>
#include <Commdlg.h>
#include <ctype.h>


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

int sstat_savefiledialog(char* _filename)
{
	char oldpath[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, oldpath);
	OPENFILENAME info;
	_filename[0] = 0;
	ZeroMemory(&info, sizeof(info));
	info.lStructSize = sizeof(info);
	info.hwndOwner = 0;
	info.lpstrFilter = 0;
	info.lpstrFile = _filename;
	info.nMaxFile = MAX_PATH;
	info.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY |OFN_OVERWRITEPROMPT;
	bool ret = GetSaveFileName(&info);
	SetCurrentDirectoryA(oldpath);
	return !ret;
}

int sstat_openfiledialog(char* _filename)
{
	char oldpath[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, oldpath);
	OPENFILENAME info;
	_filename[0] = 0;
	ZeroMemory(&info, sizeof(info));
	info.lStructSize = sizeof(info);
	info.hwndOwner = 0;
	info.lpstrFilter = 0;
	info.lpstrFile = _filename;
	info.nMaxFile = MAX_PATH;
	info.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	bool ret = GetOpenFileName(&info); // thanks for changing the cwd
	SetCurrentDirectoryA(oldpath);
	return !ret;
}

void sstat_getcwd(char* _filepath, size_t _maxsize)
{
    GetCurrentDirectoryA(_maxsize, _filepath);
	_filepath[0] = toupper(_filepath[0]);
}

#elif defined (MACOSX) || defined (__APPLE__)

// memory stuff
#include <mach/mach.h>
#include <mach/mach_init.h>
#include <mach/task.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

// save dialog stuff
#include <Carbon/Carbon.h>
#include <CoreServices/CoreServices.h>
//#include <Carbon/HIToolbox/Dialogs.h>

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

OSErr SimpleNavPutFile( OSType fileType, 
												OSType fileCreator, 
												char *filepath)
{
	OSStatus       theStatus;
	NavDialogRef   theDialog;
	NavReplyRecord theReply;
	AEDesc         aeDesc;
	FSRef          fsRefParent, fsRefDelete;
	FInfo          fileInfo;
	OSErr          err = noErr;
	
	NavDialogCreationOptions inOptions;
	NavGetDefaultDialogCreationOptions(&inOptions);
	inOptions.optionFlags = kNavDefaultNavDlogOptions & ~kNavAllowMultipleFiles;
	inOptions.saveFileName = CFStringCreateWithCString(NULL, filepath, CFStringGetSystemEncoding());
	
	theStatus = NavCreatePutFileDialog(&inOptions, fileType, fileCreator, NULL, NULL, &theDialog);
	NavDialogRun(theDialog);
	theStatus = NavDialogGetReply ( theDialog, &theReply); 
	NavDialogDispose(theDialog);
	
	if(!theReply.validRecord)
	{
	  // Assuming the user changed his/her mind? No harm; no foul.
	  // Still need to indicate that a file has not been created
	  return -1;   
	}   
                    
	err = AECoerceDesc(&theReply.selection, typeFSRef, &aeDesc);
	if(err != noErr)
		return err;
	
	err = AEGetDescData(&aeDesc, &fsRefParent, sizeof(FSRef));
	if(err != noErr)
		return err;
	
	// Get file name
	char _filename[1024];
	CFStringGetCString(theReply.saveFileName, _filename, 1024, CFStringGetSystemEncoding());
	
	// Get file path
	char _filepath[1024];
	CFURLRef theURL = CFURLCreateFromFSRef( kCFAllocatorDefault, &fsRefParent );
	CFStringRef thePath = CFURLCopyPath(theURL);
	CFRelease(theURL);
	CFStringGetCString(thePath, _filepath, 1024, CFStringGetSystemEncoding());
	CFRelease(thePath);
	
	// Merge them together
	sprintf(filepath, "%s%s", _filepath, _filename);
												
	if(filepath == NULL)
		return -1; // generic error

	return err;
}

OSErr SimpleNavGetFile(char *filepath)
{   
	OSStatus                    theStatus;
	NavDialogCreationOptions    inOptions;
	NavDialogRef                theDialog;
	NavReplyRecord              theReply;
	AEDesc                      aeDesc;
	FSRef                       fsRefParent, fsRefDelete;
	FInfo                       fileInfo;
	OSErr                       err = noErr;

	// Setup some dialog options
	NavGetDefaultDialogCreationOptions(&inOptions);
	inOptions.optionFlags = kNavDefaultNavDlogOptions & ~kNavAllowMultipleFiles;
	inOptions.saveFileName = CFStringCreateWithCString(NULL, filepath, CFStringGetSystemEncoding());
	
	// Display dialog
	theStatus = NavCreateGetFileDialog(&inOptions, NULL, NULL, NULL, NULL, NULL, &theDialog);
	NavDialogRun(theDialog);
	theStatus = NavDialogGetReply ( theDialog, &theReply); 
	NavDialogDispose(theDialog);

	if(!theReply.validRecord)
	{
		return -1;      
		// Assuming the user changed his/her mind? 
		// No harm; no foul, but need to know 
		// not to try to open the file.
	}
	
	err = AECoerceDesc(&theReply.selection, typeFSRef, &aeDesc);
	if(err != noErr)
		return err;
	
	err = AEGetDescData(&aeDesc, &fsRefParent, sizeof(FSRef));
	if(err != noErr)
		return err;
	
	// Get file path
	char _filepath[1024];
	CFURLRef theURL = CFURLCreateFromFSRef( kCFAllocatorDefault, &fsRefParent );
	CFStringRef thePath = CFURLCopyPath(theURL);
	CFRelease(theURL);
	CFStringGetCString(thePath, _filepath, 1024, CFStringGetSystemEncoding());
	CFRelease(thePath);
	
	// Merge them together
	sprintf(filepath, "%s", _filepath);
												
	if(filepath == NULL)
		return -1; // generic error

	return err;
}

int sstat_savefiledialog(char* _filename)
{
  OSErr err = noErr;
	err = SimpleNavPutFile('TEXT', 'XCEL', _filename);

	return (int)err;
}

int sstat_openfiledialog(char* _filename)
{
  OSErr err = noErr;
	err = SimpleNavGetFile(_filename);

	return (int)err;
}

void sstat_getcwd(char* _filepath, size_t _maxsize)
{
    getcwd(_filepath, _maxsize);
}



#elif defined (__LINUX__) || defined (__linux__)

#include <unistd.h>
#include <ios>
#include <iostream>
#include <fstream>
#include <string.h>
#include <gtk/gtk.h>

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

gboolean destroy_dialog(gpointer data)
{
	gtk_widget_destroy((GtkWidget*)data);
	gtk_main_quit();
}

int sstat_savefiledialog(char* _filename)
{
	gtk_init_check(NULL, NULL);

	GtkWidget* dialog;

	dialog = gtk_file_chooser_dialog_new(
		"Save File"
		, NULL
		, GTK_FILE_CHOOSER_ACTION_SAVE
		, GTK_STOCK_CANCEL
		, GTK_RESPONSE_CANCEL
		, GTK_STOCK_SAVE
		, GTK_RESPONSE_ACCEPT
		, NULL
	);

   if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
   {
      char *g_filename;
      g_filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

		strncpy(_filename, g_filename, strlen(g_filename)+1);

      g_free(g_filename);
   }
	else
	{
		g_signal_connect(GTK_DIALOG(dialog), "destroy", G_CALLBACK(gtk_main_quit), GTK_DIALOG(dialog));
		g_timeout_add(10, destroy_dialog, dialog);

		gtk_main();

		return -1;
	}

	g_signal_connect(GTK_DIALOG(dialog), "destroy", G_CALLBACK(gtk_main_quit), GTK_DIALOG(dialog));
	g_timeout_add(10, destroy_dialog, dialog);

	gtk_main();

	return 0;
}

int sstat_openfiledialog(char* _filename)
{
	gtk_init(NULL, NULL);

	GtkWidget* dialog;

	dialog = gtk_file_chooser_dialog_new(
		"Open File"
		, NULL
		, GTK_FILE_CHOOSER_ACTION_OPEN
		, GTK_STOCK_CANCEL
		, GTK_RESPONSE_CANCEL
		, GTK_STOCK_OPEN
		, GTK_RESPONSE_ACCEPT
		, NULL
	);

   if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
   {
      char *g_filename;
      g_filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

		strncpy(_filename, g_filename, strlen(g_filename)+1);

      g_free(g_filename);
   }
	else
	{
		g_signal_connect(GTK_DIALOG(dialog), "destroy", G_CALLBACK(gtk_main_quit), GTK_DIALOG(dialog));
		g_timeout_add(10, destroy_dialog, dialog);

		gtk_main();

		return -1;
	}

	g_signal_connect(GTK_DIALOG(dialog), "destroy", G_CALLBACK(gtk_main_quit), GTK_DIALOG(dialog));
	g_timeout_add(10, destroy_dialog, dialog);

	gtk_main();


	return 0;
}

void sstat_getcwd(char* _filepath, size_t _maxsize)
{
    getcwd(_filepath, _maxsize);
}

#else


long int sstat_memoryusage()
{
	return 420*1024*1024;
}

int sstat_savefiledialog(char* _filename)
{
	return -1;
}

int sstat_openfiledialog(char* _filename)
{
  return -1;
}

void sstat_getcwd(char* _filepath, size_t _maxsize)
{
    _filepath[0] = 0;
}

#endif
