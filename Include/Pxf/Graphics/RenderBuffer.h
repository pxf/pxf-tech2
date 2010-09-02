#ifndef _PXF_GRAPHICS_RENDERBUFFER_H_
#define _PXF_GRAPHICS_RENDERBUFFER_H_

#include <Pxf/Graphics/DeviceResource.h>
#include <Pxf/Graphics/GraphicsDevice.h>

namespace Pxf
{
	namespace Graphics
	{
		//! Abstract render buffer class
		class RenderBuffer : public DeviceResource
		{
		public:
			RenderBuffer()
				: DeviceResource(0)
			{ }

			RenderBuffer(GraphicsDevice* _Device, unsigned _Width, unsigned _Height)
				: DeviceResource(_Device)
				, m_Width(_Width)
				, m_Height(_Height)
			{ }

			int GetWidth() { return m_Width; }
			int GetHeight() { return m_Height; }

			bool Ready() { return m_Ready; }

		private:
			unsigned m_Width;
			unsigned m_Height;
			bool m_Ready;
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_TEXTURE_H_