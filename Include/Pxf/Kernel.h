#ifndef _PXF_KERNEL_H_
#define _PXF_KERNEL_H_

#include <Pxf/Util/Array.h>

namespace Pxf {
    class SharedLibrary;
    class Module;
    
    class Kernel
    {
    private:
        static Kernel* s_Kernel;
        
        typedef void(*DestroyModuleInstance_fun)(Pxf::Module*);
        struct ModuleEntry_t
        {
            Pxf::SharedLibrary* dynlib;
            Pxf::Module* module;
            DestroyModuleInstance_fun destroy;
            ModuleEntry_t(Pxf::Module* _Module, DestroyModuleInstance_fun _Fun)
            {
                module = _Module;
                destroy = _Fun;
                dynlib = 0;
            }
            ModuleEntry_t(Pxf::SharedLibrary* _Library, Pxf::Module* _Module, DestroyModuleInstance_fun _Fun)
            {
                module = _Module;
                destroy = _Fun;
                dynlib = _Library;
            }
        };
        
        Pxf::Util::Array<ModuleEntry_t*> m_AvailableModules;
        
        Kernel();
        Kernel(const Kernel& _Other){};
        
    public:
        static const unsigned KERNEL_VERSION;
    
        ~Kernel();
        enum SystemType
        {
            SYSTEM_TYPE_GRAPHICS = 1,
            SYSTEM_TYPE_SOUND,
            SYSTEM_TYPE_PHYSICS,
            SYSTEM_TYPE_RESOURCE_LOADER,
            SYSTEM_TYPE_SCRIPTLANG
        };
        
        static Kernel* GetInstance()
        {
            if (!s_Kernel)
                s_Kernel = new Kernel();
            return s_Kernel;
        }
        
        //Audio* GetAudiodevice();
        //Graphics* GetGraphicsDevice();
        //Physics* GetPhysicsEngine();
        //ResourceManager* GetResourceManager();
        //ScriptEngine* GetScriptEngine();
        
        static unsigned int GetKernelVersion()
        {
            return KERNEL_VERSION;
        }
        
        bool RegisterModule(const char* _FilePath, bool _OverrideBuiltin = false);
        bool RegisterModule(Pxf::Module* _Module);
        bool RegisterSystem(const char* _ModuleID, SystemType _SystemType);
        void DumpAvailableModules();
    }; // class Kernel
    
} // namespace Pxf

#endif // _PXF_KERNEL_H_