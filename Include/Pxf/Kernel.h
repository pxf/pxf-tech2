#ifndef _PXF_KERNEL_H_
#define _PXF_KERNEL_H_

#include <Pxf/Util/Array.h>

namespace Pxf {
    class Module;
    
    class Kernel
    {
    private:
        static Kernel* s_Kernel;
        
        typedef void(*DestroyModuleInstance_fun)(Pxf::Module*);
        struct ModuleEntry_t
        {
            Pxf::Module* module;
            DestroyModuleInstance_fun destroy;
            ModuleEntry_t(Pxf::Module* _Module, DestroyModuleInstance_fun dmi)
            {
                module = _Module;
                destroy = dmi;
            }
        };
        
        Pxf::Util::Array<ModuleEntry_t*> m_AvailableModules;
        
        Kernel();
        Kernel(const Kernel& _Other){};
        
    public:
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
        void DumpAvailableModules();
    }; // class Kernel
    
} // namespace Pxf

#endif // _PXF_KERNEL_H_