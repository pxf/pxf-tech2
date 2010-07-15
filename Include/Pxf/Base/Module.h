#ifndef _PXF_BASE_MODULE_H_
#define _PXF_BASE_MODULE_H_

namespace Pxf {
namespace Base
{
    class Module
    {
    private:
        unsigned m_ApiVersion;
    public:
        Module(unsigned ApiVersion)
            : m_ApiVersion(ApiVersion)
        {}
/*
        static System* CreateInstance()
        {
            return new System;
        }
*/
        unsigned GetApiVersion() const
        {
            return m_ApiVersion;
        }
    };
} // Base
} // Pxf

#endif // _PXF_BASE_MODULE_H_