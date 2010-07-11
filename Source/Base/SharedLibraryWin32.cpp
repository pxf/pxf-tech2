#include <Pxf/Base/SharedLibrary.h>

#ifdef CONF_FAMILY_WINDOWS

using namespace Pxf::Base;

SharedLibrary::SharedLibrary()
{
}

SharedLibrary::~SharedLibrary()
{
    if (m_pHandle)
        Close();
}

bool SharedLibrary::Load(const char* _File)
{
    m_pHandle = LoadLibrary(_File);
    return m_pHandle != 0;
}

bool SharedLibrary::Close()
{
    bool r = FreeLibrary(m_pHandle);
    return r;
}

void* SharedLibrary::LookupName(const char* _Name)
{
    return GetProcAddress(m_pHandle, _Name);
}

char* SharedLibrary::GetError()
{
    return "Unknown error.";
    //return GetLastError();
}

#endif // CONF_FAMILY_WINDOWS