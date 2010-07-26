#include <Pxf/Base/SharedLibrary.h>

#ifdef CONF_FAMILY_WINDOWS

#include <stdlib.h>

Pxf::SharedLibrary::SharedLibrary()
{
    m_LastError = (char*)malloc(1);
}

Pxf::SharedLibrary::~SharedLibrary()
{
    if (m_pHandle)
        Close();
        
    if (m_LastError)
        free(m_LastError);
}

bool Pxf::SharedLibrary::Load(const char* _File)
{
    m_Path = _File;
    m_pHandle = LoadLibrary(_File);
    return m_pHandle != 0;
}

bool Pxf::SharedLibrary::Close()
{
    bool r = FreeLibrary(m_pHandle);
    return r;
}

void* Pxf::SharedLibrary::LookupName(const char* _Name)
{
    return GetProcAddress(m_pHandle, _Name);
}

char* Pxf::SharedLibrary::GetError()
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, 
        GetLastError(), 
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
        (LPTSTR) &lpMsgBuf,
        0, 
        NULL);

    this->m_LastError = (char*)realloc(m_LastError, wcslen((const wchar_t*)lpMsgBuf)*sizeof(char*));
    WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)lpMsgBuf, -1, m_LastError, wcslen((const wchar_t*)lpMsgBuf)*sizeof(char*), NULL, NULL);
    LocalFree(lpMsgBuf);
    return m_LastError;
}

#endif // CONF_FAMILY_WINDOWS