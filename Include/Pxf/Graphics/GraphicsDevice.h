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

	namespace Resource
	{
		class Image;
		class Mesh;
	}

	namespace Graphics
	{
		class Window;
		class WindowSpecifications;
		class VertexBuffer;
		class NonInterleavedVertexBuffer;
		
		class Texture;
		class RenderBuffer;
		class FrameBufferObject;
		class Shader;
		class Model;
		
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
			virtual void SetModelView(Math::Mat4 *_matrix) = 0;
			virtual void Translate(Math::Vec3f _translate) = 0;
			virtual void SwapBuffers() = 0;

			// Texture
			virtual Texture* CreateEmptyTexture(int _Width,int _Height,TextureFormatStorage _Format = TEX_FORMAT_RGBA) = 0;
			virtual Texture* CreateTexture(const char* _filepath) = 0;
			virtual Texture* CreateTexture(Resource::Image* _Image) = 0;
			virtual Texture* CreateTextureFromData(const unsigned char* _datachunk, int _width, int _height, int _channels) = 0;
			virtual Texture* BindTexture(Texture* _texture) = 0;
			virtual Texture* BindTexture(Texture* _texture, unsigned int _texture_unit) = 0; // Multi-texturing
      virtual void DestroyTexture(Texture* _texture) = 0;
            
			virtual VertexBuffer* CreateVertexBuffer(VertexBufferLocation _VertexBufferLocation, VertexBufferUsageFlag _VertexBufferUsageFlag) = 0;
			virtual void DestroyVertexBuffer(VertexBuffer* _pVertexBuffer) = 0;
			virtual void DrawBuffer(VertexBuffer* _pVertexBuffer, unsigned _VertexCount) = 0;
			
			// Model
			virtual Model* CreateModel(const char* _FilePath) = 0;
			virtual Model* CreateModel(Resource::Mesh* _Mesh) = 0;

			// Buffers
			virtual RenderBuffer* CreateRenderBuffer(unsigned _Format, unsigned _Width, unsigned _Height) = 0;
			virtual void DestroyRenderBuffer(RenderBuffer* _pRenderBuffer) = 0;
			virtual void BindRenderBuffer(RenderBuffer* _pRenderBuffer) = 0;
			virtual void UnbindRenderBuffer() = 0;

			virtual FrameBufferObject* CreateFrameBufferObject() = 0;
			virtual void DestroyFrameBufferObject(FrameBufferObject* _pFrameBufferObject) = 0;
			virtual Graphics::FrameBufferObject* BindFrameBufferObject(FrameBufferObject* _pFrameBufferObject) = 0;
			virtual void UnbindFrameBufferObject() = 0;
			
			virtual Shader* CreateShader(const char* _Ident, const char* _VertexShader, const char* _FragmentShader) = 0;
			virtual void DestroyShader(Shader* _Shader) = 0;
			virtual Shader* BindShader(Shader* _Shader) = 0;
			virtual void SetUniformi(Shader* _Shader, const char* _name, int _value) = 0;
			virtual void SetUniformf(Shader* _Shader, const char* _name, float _value) = 0;
			virtual void SetUniformVec2(Shader* _Shader, const char* _name, const Math::Vec2f* _value) = 0;
			virtual void SetUniformVec3(Shader* _Shader, const char* _name, const Math::Vec3f* _value) = 0;
			virtual void SetUniformVec3v(Shader* _Shader, const char* _name, unsigned count, const Math::Vec3f* _value) = 0;
			virtual void SetUniformVec4(Shader* _Shader, const char* _name, const Math::Vec4f* _value) = 0;
			virtual void SetUniformVec4v(Shader* _Shader, const char* _name, unsigned count, const Math::Vec4f* _value) = 0;
			virtual void SetUniformMat4(Shader* _Shader, const char* _name, const Math::Mat4* _value) = 0;

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