#ifndef _PXF_GRAPHICS_OPENGL2_VERTEXBUFFERGL2_H_
#define _PXF_GRAPHICS_OPENGL2_VERTEXBUFFERGL2_H_

#include <Pxf/Graphics/VertexBuffer.h>
#include <Pxf/Graphics/DeviceType.h>
#include <Pxf/Graphics/GraphicsDevice.h> // VertexBufferType

// http://www.songho.ca/opengl/gl_vertexarray.html
// http://www.songho.ca/opengl/gl_vbo.html

namespace Pxf
{
	namespace Graphics
	{
		//! Abstract class for vertex buffer
		class VertexBufferGL2 : public VertexBuffer
		{
		protected:
			AttributeData* m_CustomVertexAttributes;
			int m_NumCustomVertexAttributes;
			unsigned m_BufferObjectId;
			unsigned m_LogTag;
		public:
			VertexBufferGL2(GraphicsDevice* _pDevice, VertexBufferLocation _VertexBufferLocation, VertexBufferUsageFlag _VertexBufferUsageFlag);
			virtual ~VertexBufferGL2();

			void CreateNewBuffer(uint32 _NumVertices, uint32 _VertexSize);
			void CreateFromBuffer(void* _Buffer,uint32 _NumVertices, uint32 _VertexSize); 

			void UpdateData(void* _Buffer, uint32 _Count, uint32 _Offset);

			void* MapData(VertexBufferAccessFlag _AccessFlag);
			void UnmapData();

			void SetCustomData(uint32 _Index, DeviceDataType _Type, bool _Normalized, uint8 _StrideOffset, uint8 _NumComponents);

			void _PreDraw();
			void _PostDraw();
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_OPENGL2_VERTEXBUFFERGL2_H_