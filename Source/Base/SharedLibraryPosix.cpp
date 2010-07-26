#include <Pxf/Base/SharedLibrary.h>
#include <Pxf/Base/Config.h>

#ifdef CONF_FAMILY_UNIX

#include <dlfcn.h>

Pxf::SharedLibrary::SharedLibrary()
    : m_pHandle(0)
{
}

Pxf::SharedLibrary::~SharedLibrary()
{
    if (m_pHandle)
        Close();
}

bool Pxf::SharedLibrary::Load(const char* _File)
{
    m_Path = _File;
    m_pHandle = dlopen(_File, RTLD_NOW);
    return m_pHandle != 0;
}

bool Pxf::SharedLibrary::Close()
{
    if (dlclose(m_pHandle) < 0)
        return false;
    else
        return true;
}

void* Pxf::SharedLibrary::LookupName(const char* _Name)
{
    return dlsym(m_pHandle, _Name);
}

char* Pxf::SharedLibrary::GetError()
{
    return dlerror();
}

#endif // CONF_FAMILY_UNIX