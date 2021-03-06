#ifndef _PXF_DERP_RENDERER_REMOTELOGWRITER_H_
#define _PXF_DERP_RENDERER_REMOTELOGWRITER_H_

#include <Pxf/Base/Logger.h>

namespace Pxf {
	namespace Network { class NetworkDevice;
	                    class Server; }
}

namespace Derp
{
	class RemoteLogWriter : public Pxf::Logger
	{
	protected:
		Pxf::Network::NetworkDevice* m_Device;
		Pxf::Network::Server* m_Server;
		unsigned m_NetLogTag;
	public:
		RemoteLogWriter(Pxf::Network::NetworkDevice* _Device, Pxf::Network::Server* _Server, unsigned _NetLogTag)
			: m_Device(_Device)
			, m_Server(_Server)
			, m_NetLogTag(_NetLogTag)
		{}

		virtual bool WriteImpl(unsigned int _Tag, const char** _TagTable, unsigned int _TagTableSize, const char* _SrcBuffer, unsigned int _SrcLength);
	};
}

#endif // _PXF_DERP_RENDERER_REMOTELOGWRITER_H_