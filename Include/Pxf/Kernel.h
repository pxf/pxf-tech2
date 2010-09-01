#ifndef _PXF_KERNEL_H_
#define _PXF_KERNEL_H_

#include <Pxf/Util/Array.h>
#include <Pxf/System.h>

namespace Pxf {
    namespace Audio { class AudioDevice; }
    namespace Input { class InputDevice; }
    namespace Graphics { class GraphicsDevice; }
    namespace Resource { class ResourceManager; 
                         class ResourceLoader; }
    
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
        
        Audio::AudioDevice* m_AudioDevice;
        Input::InputDevice* m_InputDevice;
        Graphics::GraphicsDevice* m_GraphicsDevice;
        Resource::ResourceManager* m_ResourceManager;
        
        Kernel();
        Kernel(const Kernel& _Other){};
        
    public:
        static const unsigned KERNEL_VERSION;
    
        ~Kernel();
        
        static Kernel* GetInstance()
        {
            if (!s_Kernel)
                s_Kernel = new Kernel();
            return s_Kernel;
        }
        
        void RegisterAudioDevice(Audio::AudioDevice* _Device);
        Audio::AudioDevice* GetAudioDevice();

        void RegisterInputDevice(Input::InputDevice* _Device);
        Input::InputDevice* GetInputDevice();
        
        void RegisterGraphicsDevice(Graphics::GraphicsDevice* _Device);
        Graphics::GraphicsDevice* GetGraphicsDevice();
        
        void RegisterResourceLoader(const char* _Ext, Resource::ResourceLoader* _ResourceLoader);
        Resource::ResourceManager* GetResourceManager();
        
        //void RegisterPhysicsEngine(PhysicsEngine* _Engine);
        //PhysicsEngine* GetPhysicsEngine();
        //void RegisterScriptEngine(ScriptEngine* _Engine);
        //ScriptEngine* GetScriptEngine();

        //ResourceManager* GetResourceManager();
        
        static unsigned int GetKernelVersion()
        {
            return KERNEL_VERSION;
        }
        
        bool RegisterModule(const char* _FilePath, unsigned _Filter, bool _OverrideBuiltin = false);
        bool RegisterModule(Pxf::Module* _Module);
        void DumpAvailableModules();
    }; // class Kernel
    
} // namespace Pxf

#endif // _PXF_KERNEL_H_