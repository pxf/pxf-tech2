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
		class Shader;
		class Model;
		class PrimitiveBatch;
	}

	namespace Resource
	{
		class Image;
		class Mesh;
	}

	namespace Modules {
		class DeviceGL2 : public Graphics::GraphicsDevice
		{
		public:
			DeviceGL2(Pxf::Kernel* _Kernel);
			virtual ~DeviceGL2();

			Graphics::Window* OpenWindow(Graphics::WindowSpecifications* _pWindowSpecs);
			Graphics::Window* GetWindow();
			void CloseWindow();

			Graphics::DeviceType GetDeviceType() { return Graphics::EOpenGL2; }

			void SetViewport(int _x, int _y, int _w, int _h);
			void SetProjection(Math::Mat4 *_matrix);
			void SetModelView(Math::Mat4 *_matrix);
			void SwapBuffers();
			void Translate(Math::Vec3f _translate);
			void SetDepthFunction(Graphics::DepthFuncType _DepthType);
			void SetDepthState(bool _State);

			float Print(Graphics::Font* _Font, float _X, float _Y, float _Scale, const char* _Text);

			Graphics::Texture* CreateEmptyTexture(int _Width,int _Height, Graphics::TextureFormatStorage _Format = Graphics::TEX_FORMAT_RGBA);
			Graphics::Texture* CreateTexture(const char* _filepath);
			Graphics::Texture* CreateTexture(Resource::Image* _Image);
			Graphics::Texture* CreateTextureFromData(const unsigned char* _datachunk, int _width, int _height, int _channels);
			Graphics::Texture* BindTexture(Graphics::Texture* _texture);
			Graphics::Texture* BindTexture(Graphics::Texture* _texture, unsigned int _texture_unit);
			void DestroyTexture(Graphics::Texture* _texture);

			Graphics::Texture* CreateTextureFromFramebuffer();
			Resource::Image* CreateImageFromTexture(Graphics::Texture* _texture);

			Graphics::Model* CreateModel(const char* _FilePath);
			Graphics::Model* CreateModel(Resource::Mesh* _Mesh);

			Graphics::VertexBuffer* CreateVertexBuffer(Graphics::VertexBufferLocation _VertexBufferLocation, Graphics::VertexBufferUsageFlag _VertexBufferUsageFlag);
			void DestroyVertexBuffer(Graphics::VertexBuffer* _pVertexBuffer);
			void DrawBuffer(Graphics::VertexBuffer* _pVertexBuffer, unsigned _VertexCount = 0);

			Graphics::RenderBuffer* CreateRenderBuffer(unsigned _Format, unsigned _Width, unsigned _Height);
			void DestroyRenderBuffer(Graphics::RenderBuffer* _pRenderBuffer);
			
			void BindRenderBuffer(Graphics::RenderBuffer* _pRenderBuffer);
			void UnbindRenderBuffer();

			Graphics::FrameBufferObject* CreateFrameBufferObject();
			void DestroyFrameBufferObject(Graphics::FrameBufferObject* _pFrameBufferObject);

			Graphics::FrameBufferObject* BindFrameBufferObject(Graphics::FrameBufferObject* _pFrameBufferObject);
			void UnbindFrameBufferObject();

			Graphics::FrameBufferObject* GetCurrentFrameBufferObject() { return m_CurrentFrameBufferObject; }

			Graphics::Shader* CreateShader(const char* _Ident, const char* _VertexShader, const char* _FragmentShader);
			Graphics::Shader* CreateShaderFromPath(const char* _Ident, const char* _VertexShaderPath, const char* _FragmentShaderPath);
			void DestroyShader(Graphics::Shader* _Shader);
			Graphics::Shader* BindShader(Graphics::Shader* _Shader);
			virtual void BindAttributeLocation(Graphics::Shader* _Shader, unsigned _Index, const char* _Name);
			virtual void SetUniformi(Graphics::Shader* _Shader, const char* _name, int _value);
			virtual void SetUniformf(Graphics::Shader* _Shader, const char* _name, float _value);
			virtual void SetUniformVec2(Graphics::Shader* _Shader, const char* _name, const Math::Vec2f* _value);
			virtual void SetUniformVec3(Graphics::Shader* _Shader, const char* _name, const Math::Vec3f* _value);
			virtual void SetUniformVec3v(Graphics::Shader* _Shader, const char* _name, unsigned count, const Math::Vec3f* _value);
			virtual void SetUniformVec4(Graphics::Shader* _Shader, const char* _name, const Math::Vec4f* _value);
			virtual void SetUniformVec4v(Graphics::Shader* _Shader, const char* _name, unsigned count, const Math::Vec4f* _value);
			virtual void SetUniformMat4(Graphics::Shader* _Shader, const char* _name, const Math::Mat4* _value);

		private:
			bool Init(){ return true; };
			Graphics::PrimitiveBatch* m_PrimitiveBatch;
			Graphics::Window* m_Window;
			Graphics::FrameBufferObject* m_CurrentFrameBufferObject;
			Graphics::Shader* m_CurrentShader;
			Graphics::Texture* m_BindHistory[16];
			unsigned int m_LogTag;
		};

	} // Graphics
} // Pxf

#endif //_PXF_GRAPHICS_DEVICEGL2_H_
