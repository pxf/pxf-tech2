#ifndef _PXF_GRAPHICS_FRAMEBUFFEROBJECT_H_
#define _PXF_GRAPHICS_FRAMEBUFFEROBJECT_H_

#include <Pxf/Graphics/DeviceResource.h>

namespace Pxf
{
	namespace Graphics
	{	
		class RenderBuffer;
		class GraphicsDevice;

		//! Abstract frame buffer object class
		class FrameBufferObject : public DeviceResource
		{
		public:
			FrameBufferObject(GraphicsDevice* _Device)
				: DeviceResource(_Device)
			{ }

			void AddColorAttachment(RenderBuffer* _Attachment);
			void AddDepthAttachment(RenderBuffer* _Depth);

			int GetNumColorAttachment() { return m_NumColorAttachment; }
			bool GetUseDepthAttachment() { return m_UseDepthAttachment; }
		private:
			RenderBuffer* m_ColorAttachments;
			RenderBuffer m_DepthAttachment;
			//RenderBuffer m_StencilAttachment; // future implementation

			int m_MaxColorAttachments;	// upper bound on attachments, vendor specific
			int m_NumColorAttachment;
			bool m_UseDepthAttachment;
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_TEXTURE_H_