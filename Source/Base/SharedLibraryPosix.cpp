#include <Pxf/Base/SharedLibrary.h>
#include <Pxf/Base/Config.h>

#ifdef CONF_FAMILY_UNIX

#include <dlfcn.h>

Pxf::SharedLibrary::SharedLibrary()
    : m_pHandle(0)
    , m_Path(0)
{
}

Pxf::SharedLibrary::~SharedLibrary()
{
    if (m_Path)
        delete [] m_Path;
        
    if (m_pHandle)
        Close();
}

bool Pxf::SharedLibrary::Load(const char* _File)
{
    if (m_Path)
        delete [] m_Path;
    unsigned len = strlen(_File);
    m_Path = new char[len+1];
    strncpy(m_Path, _File, len);
    m_Path[len] = 0;
    
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