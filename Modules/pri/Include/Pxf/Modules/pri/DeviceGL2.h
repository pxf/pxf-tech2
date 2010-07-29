#ifndef _PXF_GRAPHICS_DEVICEGL2_H_
#define _PXF_GRAPHICS_DEVICEGL2_H_

#include <Pxf/GraphicsDevice/GraphicsDevice.h>
#include <Pxf/GraphicsDevice/Window.h>
#include <Pxf/Modules/pri/OpenGL.h>
#include <Pxf/Modules/pri/WindowGL2.h>

namespace Pxf{
	namespace Graphics {

		class QuadBatch;
		class VertexBuffer;
		class RenderTarget;

		class DeviceGL2 : public Device
		{
		public:
			DeviceGL2();
			virtual ~DeviceGL2();

			Window* OpenWindow(WindowSpecifications* _pWindowSpecs);
			void CloseWindow();

			DeviceType GetDeviceType() { return EOpenGL2; }

			void SetViewport(int _x, int _y, int _w, int _h);
			void SetProjection(Math::Mat4 *_matrix);
			void SwapBuffers();
			void Translate(Math::Vec3f _translate);

			Texture* CreateEmptyTexture(int _Width,int _Height, TextureFormatStorage _Format = TEX_FORMAT_RGBA);
			Texture* CreateTexture(const char* _filepath);
			Texture* CreateTextureFromData(const unsigned char* _datachunk, int _width, int _height, int _channels);
			void BindTexture(Texture* _texture);
			void BindTexture(Texture* _texture, unsigned int _texture_unit);

			VertexBuffer* CreateVertexBuffer(VertexBufferLocation _VertexBufferLocation, VertexBufferUsageFlag _VertexBufferUsageFlag);
			void DestroyVertexBuffer(VertexBuffer* _pVertexBuffer);
			void DrawBuffer(VertexBuffer* _pVertexBuffer);
		private:
			Window* m_Window;
		};

	} // Graphics
} // Pxf

#endif //_PXF_GRAPHICS_DEVICEGL2_H_
