#ifndef _PXF_GRAPHICS_DEVICEGL2_H_
#define _PXF_GRAPHICS_DEVICEGL2_H_

#include <Pxf/Kernel.h>
#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Graphics/Window.h>
#include <Pxf/Modules/pri/OpenGL.h>
#include <Pxf/Modules/pri/WindowGL2.h>

namespace Pxf{
    namespace Graphics
    {
		class QuadBatch;
		class VertexBuffer;
		class RenderTarget;
		class RenderBuffer;
		class FrameBufferObject;
    }
	namespace Modules {
		class DeviceGL2 : public Graphics::GraphicsDevice
		{
		public:
			DeviceGL2(Pxf::Kernel* _Kernel);
			virtual ~DeviceGL2();

			Graphics::Window* OpenWindow(Graphics::WindowSpecifications* _pWindowSpecs);
			void CloseWindow();

			Graphics::DeviceType GetDeviceType() { return Graphics::EOpenGL2; }

			void SetViewport(int _x, int _y, int _w, int _h);
			void SetProjection(Math::Mat4 *_matrix);
			void SwapBuffers();
			void Translate(Math::Vec3f _translate);

			Graphics::Texture* CreateEmptyTexture(int _Width,int _Height, Graphics::TextureFormatStorage _Format = Graphics::TEX_FORMAT_RGBA);
			Graphics::Texture* CreateTexture(const char* _filepath);
			Graphics::Texture* CreateTextureFromData(const unsigned char* _datachunk, int _width, int _height, int _channels);
			void BindTexture(Graphics::Texture* _texture);
			void BindTexture(Graphics::Texture* _texture, unsigned int _texture_unit);

			Graphics::VertexBuffer* CreateVertexBuffer(Graphics::VertexBufferLocation _VertexBufferLocation, Graphics::VertexBufferUsageFlag _VertexBufferUsageFlag);
			void DestroyVertexBuffer(Graphics::VertexBuffer* _pVertexBuffer);
			void DrawBuffer(Graphics::VertexBuffer* _pVertexBuffer, unsigned _VertexCount = 0);

			Graphics::RenderBuffer* CreateRenderBuffer(unsigned _Format, unsigned _Width, unsigned _Height);
			Graphics::FrameBufferObject* CreateFrameBufferObject();

		private:
            bool Init(){ return true; };
			Graphics::Window* m_Window;
		};

	} // Graphics
} // Pxf

#endif //_PXF_GRAPHICS_DEVICEGL2_H_
