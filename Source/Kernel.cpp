#include <Pxf/Kernel.h>

class KernelLocatorInit
{
public:
    KernelLocatorInit()
    {
        Pxf::KernelLocator loc;
        loc.RegisterInstance(new Pxf::Kernel());
    }
};

static KernelLocatorInit init;