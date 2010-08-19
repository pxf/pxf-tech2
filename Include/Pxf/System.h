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
    
        enum SystemType : unsigned char
        {
            SYSTEM_TYPE_AUDIODEVICE = 1,
            SYSTEM_TYPE_GRAPHICS = 2,
            SYSTEM_TYPE_PHYSICS = 4,
            SYSTEM_TYPE_RESOURCE_LOADER = 8,
            SYSTEM_TYPE_SCRIPTLANG = 16
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