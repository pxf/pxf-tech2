#ifndef _PXF_GRAPHICS_DEVICERESOURCE_H_
#define _PXF_GRAPHICS_DEVICERESOURCE_H_
#include <Pxf/Base/Debug.h>
#include <Pxf/Graphics/GraphicsDevice.h>

namespace Pxf {
namespace Graphics {
	class DeviceResource
	{
	protected:
		GraphicsDevice *m_pDevice;
	public:
		DeviceResource(GraphicsDevice *_pDevice, bool _NeedsGLContext = true)
		{
			PXF_ASSERT(!_NeedsGLContext || _pDevice->GetWindow() != 0x0, "A window must be created for the OpenGL-context to be available");
			m_pDevice = _pDevice;
		}

		GraphicsDevice* GetDevice() const
		{
			return m_pDevice;
		}
	};

}}

#endif // _PXF_GRAPHICS_DEVICERESOURCE_H_