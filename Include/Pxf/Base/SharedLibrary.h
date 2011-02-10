#ifndef _PXF_BASE_SHAREDLIBRARY_H_
#define _PXF_BASE_SHAREDLIBRARY_H_

/*
	Shared libraries

	Example usage:
		auto dylib = new SharedLibrary();
		char* error = dylib->GetError();
		if(error) { puts(error); }
		dylib->Load("blah.dll");
		auto f = dylib->LookupName("do_thing");
		f();
		dylib.Close();
		delete dylib;
*/

#include <Pxf/Base/Config.h>

#ifdef CONF_FAMILY_WINDOWS
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif

namespace Pxf {
	class SharedLibrary
	{
	private:
		#ifdef CONF_FAMILY_WINDOWS
			HMODULE m_pHandle;
			char *m_LastError;
		#else
			void* m_pHandle;
		#endif
		char* m_Path;
	public:
		SharedLibrary();
		~SharedLibrary();
		bool Load(const char* _File);
		bool Close();
		void* LookupName(const char* _Name);
		char* GetError();
		const char* GetFilePath() const
		{
			return m_Path;
		}
	};
}

#endif // _PXF_BASE_SHAREDLIBRARY_H_