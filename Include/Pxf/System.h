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
        unsigned m_Identifier;
        virtual bool Init() = 0;
    public:
        System(Kernel* _Kernel, unsigned _Identifier)
        {
            m_Kernel = _Kernel;
            m_Identifier = _Identifier;
        }

        virtual ~System()
        {}
        
        unsigned GetIdentifier() const
        {
            return m_Identifier;
        }
    };
}

#endif // _PXF_SYSTEM_H_