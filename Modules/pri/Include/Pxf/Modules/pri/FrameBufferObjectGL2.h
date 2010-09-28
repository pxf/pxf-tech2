#ifndef _PXF_GRAPHICS_FRAMEBUFFEROBJECTGL2_H_
#define _PXF_GRAPHICS_FRAMEBUFFEROBJECTGL2_H_

#include <Pxf/Graphics/FrameBufferObject.h>
#include <Pxf/Modules/pri/OpenGL.h>

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
			FrameBufferObjectGL2(Graphics::GraphicsDevice* _Device);
			virtual ~FrameBufferObjectGL2();

			void Attach(Graphics::Texture* _Texture, const unsigned _Attachment, bool _GenMipmaps);
			void Attach(Graphics::RenderBuffer* _Buffer, const unsigned _Attachment);
			void Detach(const unsigned _Attachment);	

			void DetachAll();
			
			int GetNumAttached();
			static bool CheckFBO(unsigned _status);

			unsigned GetHandle() { return m_Handle; }
		private:
			void _Configure();	// check max attachments 
			unsigned m_Handle;
			unsigned m_LogTag;
			//GLenum m_Attachments[16];
		};
	} // Graphics
} // Pxf

#endif //_PXF_GRAPHICS_FRAMEBUFFEROBJECTGL2_H_
