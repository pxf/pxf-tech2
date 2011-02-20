#ifndef _PXF_IPHONE_GRAPHICS_RENDERBUFFERGLES_H_
#define _PXF_IPHONE_GRAPHICS_RENDERBUFFERGLES_H_

#include <Pxf/Graphics/RenderBuffer.h>

namespace Pxf{
    namespace Graphics
    {
		class GraphicsDevice;
    }

	namespace Modules {
		class RenderBufferGLES : public Graphics::RenderBuffer
		{
		public:
			RenderBufferGLES(Graphics::GraphicsDevice* _Device, unsigned _Format, unsigned _Width, unsigned _Height)
				: RenderBuffer(_Device,_Width,_Height)
				, m_Format(_Format)
				, m_LogTag(0)
			{ _InitBuffer(); }

			virtual ~RenderBufferGLES();

			unsigned GetHandle() { return m_Handle; }
			unsigned GetFormat() { return m_Format; }

			void ReleaseBuffer();

		private:
			void _InitBuffer();

			unsigned m_Handle;
			unsigned m_Format;
			unsigned m_LogTag;
		};

	} // Graphics
} // Pxf

#endif //_PXF_IPHONE_GRAPHICS_RENDERBUFFERGLES_H_
