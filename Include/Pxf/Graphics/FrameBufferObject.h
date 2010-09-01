#ifndef _PXF_GRAPHICS_FRAMEBUFFEROBJECT_H_
#define _PXF_GRAPHICS_FRAMEBUFFEROBJECT_H_

#include <Pxf/Graphics/DeviceResource.h>
#include <Pxf/Graphics/RenderBuffer.h>
#include <Pxf/Graphics/GraphicsDevice.h>

namespace Pxf
{
	namespace Graphics
	{	
		//! Abstract frame buffer object class
		class FrameBufferObject : public DeviceResource
		{
		public:
			FrameBufferObject(GraphicsDevice* _Device)
				: DeviceResource(_Device)
			{ }

			//virtual ~FrameBufferObject();

			virtual void AddColorAttachment(RenderBuffer* _Attachment) = 0;
			virtual void AddDepthAttachment(RenderBuffer* _Depth) = 0;

			virtual void DetachColor(unsigned _ID) = 0;
			virtual void DetachDepth(unsigned _ID) = 0;

			int GetNumColorAttachment() { return m_NumColorAttachment; }
			bool GetUseDepthAttachment() { return m_UseDepthAttachment; }
		private:
			RenderBuffer* m_ColorAttachments;
			RenderBuffer* m_DepthAttachment;
			//RenderBuffer m_StencilAttachment; // future implementation

			int m_MaxColorAttachments;	// upper bound on attachments, vendor specific
			int m_NumColorAttachment;
			bool m_UseDepthAttachment;	
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_TEXTURE_H_