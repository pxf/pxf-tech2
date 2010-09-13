#ifndef _PXF_MODULE_H_
#define _PXF_MODULE_H_

#define REGISTER_MODULE(MODULE, MODULEIDENT, KERNELV, MODULEV)							\
								class RegisterModule##MODULE							\
								{														\
								private:												\
									Pxf::Module* m_Module;								\
								public:													\
									RegisterModule##MODULE() : m_Module(0)				\
									{													\
										Pxf::Kernel* k = Pxf::Kernel::GetInstance();	\
										m_Module = new MODULE(MODULEIDENT, KERNELV, MODULEV); \
										k->RegisterModule(m_Module);					\
									}													\
								};														\
								static RegisterModule##MODULE g_GlobalInitializer##MODULE;

#include <Pxf/Base/Platform.h>
#include <Pxf/System.h>

namespace Pxf {
	class Kernel;
	
	class Module
	{
	private:
		unsigned m_KernelVersion;
		unsigned m_ApiVersion;
		const char* m_Identifier;
	public:
		static const unsigned MODULE_VERSION;
	
		Module(const char* _Identifier, unsigned _KernelVersion, unsigned _ApiVersion)
			: m_Identifier(_Identifier)
			, m_KernelVersion(_KernelVersion)
			, m_ApiVersion(_ApiVersion)
		{
			Platform platform; // initialize platform for timers etc.
		}

		virtual bool RegisterSystem(Pxf::Kernel* _Kernel, unsigned _SystemType) = 0;

		static void DestroySystem(Pxf::System* _System)
		{
			if (_System)
				delete _System;
		}

		unsigned GetKernelVersion() const
		{
			return m_KernelVersion;
		}

		unsigned GetApiVersion() const
		{
			return m_ApiVersion;
		}
		
		const char* GetIdentifier() const
		{
			return m_Identifier;
		}
	};
} // Pxf

#endif // _PXF_MODULE_H_