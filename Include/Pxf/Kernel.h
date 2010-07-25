#ifndef _PXF_KERNEL_H_
#define _PXF_KERNEL_H_

namespace Pxf {
    class Module;
    
    class Kernel
    {
    private:
        static Kernel* s_Kernel;
        
        /*
        typedef void*(*RegisterSystems_fun)(Kernel*);
        typedef void*(*Destroy_fun)(System*);
        struct Module_t
        {
            const char* module;
            unsigned type;
            unsigned ident;
            Destroy_fun destroy;
        }
        */

        // Pxf::Util::Array< Pxf::Util::Stack<Module>*> > m_Modules;
        //m_Modules[MODULE_TYPE_GRAPHICS].top(), pushback(), pushfront()
    public:
        Kernel();
        ~Kernel();
        enum ModuleType
        {
            MODULE_TYPE_GRAPHICS = 1,
            MODULE_TYPE_SOUND,
            MODULE_TYPE_PHYSICS,
            MODULE_TYPE_RESOURCE_LOADER,
            MODULE_TYPE_SCRIPTLANG
        };
        
        static Kernel* GetInstance()
        {
            if (!s_Kernel)
                s_Kernel = new Kernel();
            return s_Kernel;
        }
        
        static unsigned int GetEngineVersion()
        {
            return 1;
        }
        
        bool RegisterModule(const char* _FilePath);
        bool RegisterModule(Pxf::Module* _Module);
        void SetPreferredModule(ModuleType _ModuleType, const char* _ModuleID);
    }; // class Kernel
    
} // namespace Pxf

#endif // _PXF_KERNEL_H_