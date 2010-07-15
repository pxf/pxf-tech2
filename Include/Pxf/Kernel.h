#ifndef _PXF_KERNEL_H_
#define _PXF_KERNEL_H_

/*
// Pxf::Kernel::GetEngineVersion();
static const int SampleModule_Type = 0; //Pxf::Kernel::MODULE_TYPE_GRAPHICS
// Pxf::Kernel::RegisterModule(MODULE_TYPE_GRAPHICS, "module.dll")
// Pxf::Kernel::SetPreferredModule(MODULE_TYPE_GRAPHICS, "OPENGL2)
*/

namespace Pxf {
    class Kernel
    {
    private:
    public:
        enum ModuleTypes
        {
            MODULE_TYPE_GRAPHICS = 1,
            MODULE_TYPE_SOUND,
            MODULE_TYPE_PHYSICS,
            MODULE_TYPE_RESOURCE_LOADER,
            MODULE_TYPE_SCRIPTLANG
        };
        
        static unsigned int GetEngineVersion()
        {
            return 1;
        }
        
        
    }; // class Kernel
    
    class KernelLocator
    {
    private:
        Kernel* m_Kernel;
    public:
        KernelLocator()
        {}
        
        Kernel* GetInstance() const
        {
            return m_Kernel;
        }
        
        void RegisterInstance(Kernel* _Kernel)
        {
            m_Kernel = _Kernel;
        }
        
    }; // class KernelLocator
    
} // namespace Pxf

#endif // _PXF_KERNEL_H_