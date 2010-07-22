#ifndef _PXF_BASE_MODULE_H_
#define _PXF_BASE_MODULE_H_

namespace Pxf {
    class Kernel;
namespace Base
{
    class Module
    {
    private:
        unsigned m_KernelVersion;
        unsigned m_ApiVersion;
    public:
        Module(unsigned KernelVersion, unsigned ApiVersion)
            : m_KernelVersion(KernelVersion)
            , m_ApiVersion(ApiVersion)
        {}

        virtual void RegisterSystems(Pxf::Kernel* _Kernel) = 0;

        unsigned GetKernelVersion() const
        {
            return m_KernelVersion;
        }

        unsigned GetApiVersion() const
        {
            return m_ApiVersion;
        }
    };
} // Base
} // Pxf

#endif // _PXF_BASE_MODULE_H_