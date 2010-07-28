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
            SYSTEM_TYPE_SOUND = 2,
            SYSTEM_TYPE_PHYSICS = 4,
            SYSTEM_TYPE_RESOURCE_LOADER = 8,
            SYSTEM_TYPE_SCRIPTLANG = 16
        };
        
        static Kernel* GetInstance()
        {
            if (!s_Kernel)
                s_Kernel = new Kernel();
            return s_Kernel;
        }
        
        //void RegisterAudioDevice(AudioDevice* _Device);
        //AudioDevice* GetAudioDevice();
        //void RegisterGraphicsDevice(GraphicsDevice* _Device);
        //GraphicsDevice* GetGraphicsDevice();
        //void RegisterPhysicsEngine(PhysicsEngine* _Engine);
        //PhysicsEngine* GetPhysicsEngine();
        //void RegisterScriptEngine(ScriptEngine* _Engine);
        //ScriptEngine* GetScriptEngine();

        //ResourceManager* GetResourceManager();
        
        static unsigned int GetKernelVersion()
        {
            return KERNEL_VERSION;
        }
        
        bool RegisterModule(const char* _FilePath, bool _OverrideBuiltin = false);
        bool RegisterModule(Pxf::Module* _Module);
        bool RegisterSystem(const char* _ModuleID, SystemType _SystemType, unsigned _Identifier = 0);
        void DumpAvailableModules();
    }; // class Kernel
    
} // namespace Pxf

#endif // _PXF_KERNEL_H_