#ifndef _PXF_GRAPHICS_GRAPHICSDEVICE_H_
#define _PXF_GRAPHICS_GRAPHICSDEVICE_H_

#include <Pxf/System.h>

#include <Pxf/Math/Math.h>
#include <Pxf/Math/Vector.h>
#include <Pxf/Math/Matrix.h>
#include <Pxf/Graphics/DeviceDefs.h>
#include <Pxf/Graphics/DeviceType.h>
#include <Pxf/Graphics/TextureDefs.h>
#include <Pxf/Graphics/VertexBufferDefs.h>

namespace Pxf
{
    class Kernel;
	namespace Graphics
	{
		class Window;
		class WindowSpecifications;
		class VertexBuffer;
		class NonInterleavedVertexBuffer;
		
		class Texture;
		class RenderBuffer;
		class FrameBufferObject;
		
		class GraphicsDevice : public System
		{
		public:
            GraphicsDevice(Pxf::Kernel* _Kernel, const char* _Identifier)
                : System(_Kernel, Pxf::System::SYSTEM_TYPE_GRAPHICSDEVICE, _Identifier)
            {}

			// Windowing
			virtual Window* OpenWindow(WindowSpecifications* _pWindowSpecs) = 0;
			virtual void CloseWindow() = 0;

			// Device
			virtual DeviceType GetDeviceType() = 0;

			// Graphics
			virtual void SetViewport(int _x, int _y, int _w, int _h) = 0;
			virtual void SetProjection(Math::Mat4 *_matrix) = 0;
			virtual void Translate(Math::Vec3f _translate) = 0;
			virtual void SwapBuffers() = 0;

			// Texture
			virtual Texture* CreateEmptyTexture(int _Width,int _Height,TextureFormatStorage _Format = TEX_FORMAT_RGBA) = 0;
			virtual Texture* CreateTexture(const char* _filepath) = 0;
			virtual Texture* CreateTextureFromData(const unsigned char* _datachunk, int _width, int _height, int _channels) = 0;
			virtual Texture* BindTexture(Texture* _texture) = 0;
			virtual Texture* BindTexture(Texture* _texture, unsigned int _texture_unit) = 0; // Multi-texturing
            
			virtual VertexBuffer* CreateVertexBuffer(VertexBufferLocation _VertexBufferLocation, VertexBufferUsageFlag _VertexBufferUsageFlag) = 0;
			virtual void DestroyVertexBuffer(VertexBuffer* _pVertexBuffer) = 0;
			virtual void DrawBuffer(VertexBuffer* _pVertexBuffer, unsigned _VertexCount) = 0;
			
			virtual RenderBuffer* CreateRenderBuffer(unsigned _Format, unsigned _Width, unsigned _Height) = 0;
			virtual void DestroyRenderBuffer(RenderBuffer* _pRenderBuffer) = 0;
			virtual void BindRenderBuffer(RenderBuffer* _pRenderBuffer) = 0;
			virtual void UnbindRenderBuffer() = 0;

			virtual FrameBufferObject* CreateFrameBufferObject() = 0;
			virtual void DestroyFrameBufferObject(FrameBufferObject* _pFrameBufferObject) = 0;
			virtual Graphics::FrameBufferObject* BindFrameBufferObject(FrameBufferObject* _pFrameBufferObject) = 0;
			virtual void UnbindFrameBufferObject() = 0;
			
			bool Ready() { return m_Ready; }
		private:
			bool m_Ready;
			
			// Need the name VertexBuffer for draw calls...?
			// Or should *VertexArray inherit from Drawable?

			// QuadBatch : public PrimitiveBatch public : Drawable
			// 

		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_GRAPHICSDEVICE_H_