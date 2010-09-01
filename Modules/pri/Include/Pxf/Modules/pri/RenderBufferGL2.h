#ifndef _PXF_GRAPHICS_RENDERBUFFERGL2_H_
#define _PXF_GRAPHICS_RENDERBUFFERGL2_H_

#include <Pxf/Graphics/RenderBuffer.h>

namespace Pxf{
    namespace Graphics
    {
		class GraphicsDevice;
    }

	namespace Modules {
		class RenderBufferGL2 : public Graphics::RenderBuffer
		{
		public:
			RenderBufferGL2(Graphics::GraphicsDevice* _Device, unsigned _Format, unsigned _Width, unsigned _Height)
				: RenderBuffer(_Device,_Width,_Height)
				, m_Format(_Format)
			{ _InitBuffer(); }

			unsigned GetID() { return m_ID; }
			unsigned GetFormat() { return m_Format; }

			void ReleaseBuffer();
		private:
			void _InitBuffer();

			unsigned m_ID;
			unsigned m_Format;
		};

	} // Graphics
} // Pxf

#endif //_PXF_GRAPHICS_RENDERBUFFERGL2_H_
