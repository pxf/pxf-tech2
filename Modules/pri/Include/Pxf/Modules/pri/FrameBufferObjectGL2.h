#ifndef _PXF_GRAPHICS_FRAMEBUFFEROBJECTGL2_H_
#define _PXF_GRAPHICS_FRAMEBUFFEROBJECTGL2_H_

#include <Pxf/Graphics/FrameBufferObject.h>

namespace Pxf{
    namespace Graphics
    {
		class GraphicsDevice;
		class RenderBuffer;
    }

	namespace Modules {
		class FrameBufferObjectGL2 : public Graphics::FrameBufferObject
		{
		public:
			FrameBufferObjectGL2(Graphics::GraphicsDevice* _Device)
				: FrameBufferObject(_Device)
			{}
			~FrameBufferObjectGL2();

			void AddColorAttachment(Graphics::RenderBuffer* _Attachment);
			void AddDepthAttachment(Graphics::RenderBuffer* _Depth);

			void DetachColor(unsigned _ID);
			void DetachDepth(unsigned _ID);
		private:
			void _DetachAll();

		};
	} // Graphics
} // Pxf

#endif //_PXF_GRAPHICS_FRAMEBUFFEROBJECTGL2_H_
