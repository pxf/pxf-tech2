#ifndef _PXF_BASE_MODULE_H_
#define _PXF_BASE_MODULE_H_

namespace Pxf {
    class Kernel;
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

        virtual void RegisterSystems(Pxf::Kernel* _Kernel) = 0;

        unsigned GetApiVersion() const
        {
            return m_ApiVersion;
        }
    };
} // Base
} // Pxf

#endif // _PXF_BASE_MODULE_H_