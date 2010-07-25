#ifndef _PXF_BASE_MODULE_H_
#define _PXF_BASE_MODULE_H_

// Does not work because the module is compiled as a static library.
#define REGISTER_MODULE(MODULE, KERNELV, MODULEV) \
                                class RegisterModule                                       \
                                {                                                          \
                                private:                                                   \
                                    Pxf::Base::Module* m_Module;                           \
                                public:                                                    \
                                    RegisterModule() : m_Module(0)                         \
                                    {                                                      \
                                        Pxf::Kernel k;                                     \
                                        m_Module = new MODULE(#MODULE, KERNELV, MODULEV);           \
                                        k.RegisterModule(m_Module);                        \
                                    }                                                      \
                                    ~RegisterModule()                                      \
                                    {                                                      \
                                        if (m_Module)                                      \
                                            delete m_Module;                               \
                                    }                                                      \
                                };                                                         \
                                static RegisterModule g_GlobalInitializer;
namespace Pxf {
    class Kernel;
    class System;
namespace Base
{
    class Module
    {
    private:
        unsigned m_KernelVersion;
        unsigned m_ApiVersion;
        const char* m_Identifier;
    public:
        Module(const char* _Identifier, unsigned _KernelVersion, unsigned _ApiVersion)
            : m_Identifier(_Identifier)
            , m_KernelVersion(_KernelVersion)
            , m_ApiVersion(_ApiVersion)
        {}

        virtual void RegisterSystems(Pxf::Kernel* _Kernel) = 0;

        static void DestroySystem(Pxf::System* _System)
        {
            if (_System)
                delete _System;
        }

        unsigned GetKernelVersion() const
        {
            return m_KernelVersion;
        }

        unsigned GetApiVersion() const
        {
            return m_ApiVersion;
        }
        
        const char* GetIdentifier() const
        {
            return m_Identifier;
        }
    };
} // Base
} // Pxf

#endif // _PXF_BASE_MODULE_H_