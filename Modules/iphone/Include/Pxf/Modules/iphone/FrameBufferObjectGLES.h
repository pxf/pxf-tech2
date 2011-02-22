#ifndef _PXF_IPHONE_GRAPHICS_FRAMEBUFFEROBJECTGLES_H_
#define _PXF_IPHONE_GRAPHICS_FRAMEBUFFEROBJECTGLES_H_

#include <Pxf/Graphics/FrameBufferObject.h>
#include <Pxf/Modules/iphone/OpenGL.h>

namespace Pxf{
	namespace Graphics
	{
		class GraphicsDevice;
		class RenderBuffer;
	}

	namespace Modules {
		class FrameBufferObjectGLES : public Graphics::FrameBufferObject
		{
		public:
			FrameBufferObjectGLES(Graphics::GraphicsDevice* _Device);
			virtual ~FrameBufferObjectGLES();

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

#endif //_PXF_IPHONE_GRAPHICS_FRAMEBUFFEROBJECTGLES_H_
