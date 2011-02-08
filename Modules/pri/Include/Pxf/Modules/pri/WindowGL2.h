#ifndef _PXF_GRAPHICS_WINDOWGL2_H_
#define _PXF_GRAPHICS_WINDOWGL2_H_

#include <Pxf/Graphics/Window.h>
#include <Pxf/Graphics/WindowSpecifications.h>
#include <Pxf/Base/Types.h>

namespace Pxf{
	namespace Graphics {

		class WindowGL2 : public Window
		{
		public:
			WindowGL2(GraphicsDevice* _Device, WindowSpecifications *_window_spec);
			virtual ~WindowGL2();

			bool Open();
			bool Close();
			void Swap();

			void SetResizeCallback(WindowSizeCallback fun);
			void SetTitle(const char *_title);
			void SetWindowSize(int _Width, int _Height);

			int GetFPS();
			int GetWidth();
			int GetHeight();
			float GetAspectRatio();
			const char* GetContextTypeName();

			bool IsOpen();
			bool IsActive();
			bool IsMinimized();
		private:
			int m_width, m_height;
			bool m_fullscreen, m_resizeable, m_vsync;

			// Bit settings
			int m_bits_r, m_bits_g, m_bits_b;
			int m_bits_alpha, m_bits_depth, m_bits_stencil;

			// FSAA
			int m_fsaa_samples;

			// FPS
			int64 m_fps_laststamp;
			int m_fps, m_fps_count;
			unsigned int m_LogTag;
		};

	} // Graphics
} // Pxf

#endif //_PXF_GRAPHICS_WINDOWGL_H_
