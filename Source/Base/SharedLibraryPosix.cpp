#include <Pxf/Base/SharedLibrary.h>
#include <Pxf/Base/Config.h>

#ifdef CONF_FAMILY_UNIX

#include <dlfcn.h>
using namespace Pxf::Base;

SharedLibrary::SharedLibrary()
    : m_pHandle(0)
{
}

SharedLibrary::~SharedLibrary()
{
    if (m_pHandle)
        Close();
}

bool SharedLibrary::Load(const char* _File)
{
    m_pHandle = dlopen(_File, RTLD_NOW);
    return m_pHandle != 0;
}

bool SharedLibrary::Close()
{
    if (dlclose(m_pHandle) < 0)
        return false;
    else
        return true;
}

void* SharedLibrary::LookupName(const char* _Name)
{
    return dlsym(m_pHandle, _Name);
}

char* SharedLibrary::GetError()
{
    return dlerror();
}

#endif // CONF_FAMILY_UNIX