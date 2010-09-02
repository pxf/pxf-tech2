#ifndef _PXF_GRAPHICS_DEVICERESOURCE_H_
#define _PXF_GRAPHICS_DEVICERESOURCE_H_

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

		GraphicsDevice* GetDevice() const
		{
			return m_pDevice;
		}
	};

}}

#endif // _PXF_GRAPHICS_DEVICERESOURCE_H_