#ifndef _PXF_SYSTEM_H_
#define _PXF_SYSTEM_H_

namespace Pxf
{
    // Forward declarations
    class Kernel;

    //! Abstract system
    class System
    {
    protected:
        Kernel* m_Kernel;
        const char* m_Identifier;
        unsigned m_SystemType;
        virtual bool Init() = 0;
    public:
    
        enum SystemType
        {
            SYSTEM_TYPE_AUDIODEVICE = 1,
            SYSTEM_TYPE_INPUTDEVICE = 2,
            SYSTEM_TYPE_GRAPHICSDEVICE = 4,
            SYSTEM_TYPE_PHYSICSENGINE = 8,
            SYSTEM_TYPE_RESOURCE_LOADER = 16,
            SYSTEM_TYPE_SCRIPTLANG = 32,
			SYSTEM_TYPE_NETWORKDEVICE = 64
        };
    
        System(Kernel* _Kernel, unsigned _SystemType, const char* _Identifier)
        {
            m_Kernel = _Kernel;
            m_SystemType = _SystemType;
            m_Identifier = _Identifier;
        }

        virtual ~System()
        {}
        
        Kernel* GetKernel() const
        {
            return m_Kernel;
        }
        
        unsigned GetSystemType() const
        {
            return m_SystemType;
        }
        
        const char* GetIdentifier() const
        {
            return m_Identifier;
        }
    };
}

#endif // _PXF_SYSTEM_H_