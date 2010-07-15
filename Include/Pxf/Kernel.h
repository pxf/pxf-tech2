#ifndef _PXF_KERNEL_H_
#define _PXF_KERNEL_H_

/*
// Pxf::Kernel::GetEngineVersion();
static const int SampleModule_Type = 0; //Pxf::Kernel::MODULE_TYPE_GRAPHICS
// Pxf::Kernel::RegisterModule(MODULE_TYPE_GRAPHICS, "module.dll")
// Pxf::Kernel::SetPreferredModule(MODULE_TYPE_GRAPHICS, "OPENGL2")
*/

namespace Pxf {
    class Kernel
    {
    private:
        static Kernel* s_Kernel;
        // Pxf::Util::Array< Pxf::Util::Stack<Module>*> > m_Modules;
        //m_Modules[MODULE_TYPE_GRAPHICS].top(), pushback(), pushfront()
    public:
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
        
        bool RegisterModule(ModuleType _ModuleType, const char* _FilePath);
        void SetPreferredModule(ModuleType _ModuleType, const char* _ModuleID);
    }; // class Kernel
    
} // namespace Pxf

#endif // _PXF_KERNEL_H_