#include <Pxf/Kernel.h>
#include <Pxf/Base/Platform.h>
#include <Pxf/Util/String.h>
#include <Pxf/Modules/iphone/WindowIOS.h>
#include <Pxf/Base/Debug.h>

#ifdef CONF_PLATFORM_MACOSX
#include <Carbon/Carbon.h>
#endif
#include <Pxf/Modules/iphone/OpenGL.h>
#include <Pxf/Modules/iphone/UniGL.h>

using namespace Pxf;
using namespace Pxf::Graphics;
using Util::String;

int WindowIOS::GetWidth() { return m_width; }
int WindowIOS::GetHeight() {return m_height; }
float WindowIOS::GetAspectRatio() { return ((float)m_width / (float)m_height); }

bool g_ShouldCloseWindow = false;
int GLFWCALL on_window_close()
{
	g_ShouldCloseWindow = true;
	return false;
}

WindowIOS::WindowIOS(GraphicsDevice* _Device, WindowSpecifications *_window_spec)
	: Window(_Device)
{
	m_LogTag = _Device->GetKernel()->CreateTag("gfx");

	// Window settings
	m_width = _window_spec->Width;
	m_height = _window_spec->Height;
	m_fullscreen = _window_spec->Fullscreen;
	m_resizeable = _window_spec->Resizeable;
	m_vsync = _window_spec->VerticalSync;
	m_fsaa_samples = _window_spec->FSAASamples;

	// Colorbits
	switch (_window_spec->ColorBits)
	{
	case 16:
		m_bits_r = 5;
		m_bits_g = 6;
		m_bits_b = 5;
		break;
	case 24:
		m_bits_r = 8;
		m_bits_g = 8;
		m_bits_b = 8;
		break;
	default:
		GetDevice()->GetKernel()->Log(m_LogTag, "Could not initiate widow, invalid number of colorbits (%d)", _window_spec->ColorBits);
	}

	// Other buffer bits settings
	m_bits_alpha = _window_spec->AlphaBits;
	m_bits_depth = _window_spec->DepthBits;
	m_bits_stencil = _window_spec->StencilBits;

	// FPS
	m_fps = 0;
	m_fps_count = 0;
	m_fps_laststamp = Platform::GetTime();
}

WindowIOS::~WindowIOS()
{
	Close();
}

bool WindowIOS::Open()
{
	return false;
}

bool WindowIOS::Close()
{
	if(!IsOpen())
		return false;

	// closeWindow();
	return true;
}

void WindowIOS::Swap()
{
	if (IsOpen())
	{
		int64 t_current_time = Platform::GetTime();
		int64 diff = t_current_time - m_fps_laststamp;
		if (diff >= 1000)
		{
			m_fps = m_fps_count;
			m_fps_count = 0;
			m_fps_laststamp = t_current_time;
		}

		//SwapBuffers();
		m_fps_count += 1;
	}
}

bool WindowIOS::IsOpen()
{
	bool windowIsOpen = false;
	if (windowIsOpen && !g_ShouldCloseWindow)
		return true;
	else
		return false;
}

bool WindowIOS::IsActive()
{
	if (IsOpen())
	{
		bool windowIsActive = false;
		if (windowIsActive)
			return true;
	}
	return false;
}

bool WindowIOS::IsMinimized()
{
	if (IsOpen())
	{
		bool windowIsIconified = false;
		if (windowIsIconified)
			return true;
	}

	return false;
}

void WindowIOS::SetResizeCallback(WindowSizeCallback fun)
{
//	glfwSetWindowSizeCallback(fun);
}

void WindowIOS::SetWindowSize(int _Width, int _Height)
{
	m_width = _Width;
	m_height = _Height;
	//glfwSetWindowSize(_Width, _Height);
}

void WindowIOS::SetTitle(const char *_title)
{
	if (IsOpen())
	{
		//glfwSetWindowTitle(_title);
	}
}

int WindowIOS::GetFPS()
{
	return m_fps;
}

const char* WindowIOS::GetContextTypeName()
{
	return "OpenGL ES";
}
