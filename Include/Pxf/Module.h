#ifndef _PXF_MODULE_H_
#define _PXF_MODULE_H_

#define REGISTER_MODULE(MODULE, KERNELV, MODULEV) \
                                class RegisterModule##MODULE                               \
                                {                                                          \
                                private:                                                   \
                                    Pxf::Module* m_Module;                                 \
                                public:                                                    \
                                    RegisterModule##MODULE() : m_Module(0)                 \
                                    {                                                      \
                                        Pxf::Kernel* k = Pxf::Kernel::GetInstance();       \
                                        m_Module = new MODULE(#MODULE, KERNELV, MODULEV);  \
                                        k->RegisterModule(m_Module);                       \
                                    }                                                      \
                                };                                                         \
                                static RegisterModule##MODULE g_GlobalInitializer##MODULE;
namespace Pxf {
    class Kernel;
    class System;
    
    class Module
    {
    private:
        unsigned m_KernelVersion;
        unsigned m_ApiVersion;
        const char* m_Identifier;
    public:
        static const unsigned MODULE_VERSION;
    
        Module(const char* _Identifier, unsigned _KernelVersion, unsigned _ApiVersion)
            : m_Identifier(_Identifier)
            , m_KernelVersion(_KernelVersion)
            , m_ApiVersion(_ApiVersion)
        {}

        virtual bool RegisterSystem(Pxf::Kernel* _Kernel, unsigned _SystemType, unsigned _Identifier = 0) = 0;

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
} // Pxf

#endif // _PXF_MODULE_H_