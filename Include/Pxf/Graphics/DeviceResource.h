#ifndef _PXF_GRAPHICS_DEVICERESOURCE_
#define _PXF_GRAPHICS_DEVICERESOURCE_

namespace Pxf {
namespace Graphics {

	class GraphicsDevice;

	class DeviceResource
	{
	protected:
		GraphicsDevice *m_pDevice;
	public:
		DeviceResource(GraphicsDevice *_pDevice)
		{
			m_pDevice = _pDevice;
		}
	};

}}

#endif // _PXF_GRAPHICS_DEVICERESOURCE_H_