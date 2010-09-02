#ifndef _PXF_GRAPHICS_FRAMEBUFFEROBJECT_H_
#define _PXF_GRAPHICS_FRAMEBUFFEROBJECT_H_

#include <Pxf/Graphics/DeviceResource.h>
#include <Pxf/Graphics/RenderBuffer.h>
#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Graphics/Texture.h>
#include <vector>

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
				, m_NumColorAttachment(0)
				, m_MaxColorAttachments(0)
				, m_UseStencilAttachment(false)
				, m_UseDepthAttachment(false)
			{ }

			//virtual void AddColorAttachment(RenderBuffer* _Attachment, unsigned _ID) = 0;
			virtual void AddColorAttachment(Texture* _Texture, unsigned _ID,  const bool _GenMipmaps) = 0;
			virtual void AddDepthAttachment(RenderBuffer* _Depth) = 0;
			virtual void AttachStencil(RenderBuffer* _Stencil) = 0;

			virtual void DetachColor(unsigned _ID) = 0;
			virtual void DetachDepth() = 0;
			virtual void DetachStencil() = 0;

			virtual void DetachAll() = 0;

			int GetNumColorAttachment() { return m_NumColorAttachment; }
			bool GetUseDepthAttachment() { return m_UseDepthAttachment; }

			bool IsComplete() { return m_Complete; }
		protected:
			int m_MaxColorAttachments;	// upper bound on attachments, vendor specific
			int m_NumColorAttachment;
			
			bool m_UseDepthAttachment;	
			bool m_UseStencilAttachment;
			bool m_Complete;
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_TEXTURE_H_