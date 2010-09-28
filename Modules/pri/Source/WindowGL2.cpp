#include <Pxf/Kernel.h>
#include <Pxf/Base/Platform.h>
#include <Pxf/Util/String.h>
#include <Pxf/Modules/pri/WindowGL2.h>
#include <Pxf/Base/Debug.h>

#ifdef CONF_PLATFORM_MACOSX
#include <Carbon/Carbon.h>
#endif
#include <Pxf/Modules/pri/OpenGL.h>
#include <Pxf/Modules/pri/UniGL.h>

using namespace Pxf;
using namespace Pxf::Graphics;
using Util::String;

int WindowGL2::GetWidth() { return m_width; }
int WindowGL2::GetHeight() {return m_height; }
float WindowGL2::GetAspectRatio() { return ((float)m_width / (float)m_height); }

bool g_ShouldCloseWindow = false;
int GLFWCALL on_window_close()
{
	g_ShouldCloseWindow = true;
	return false;
}

WindowGL2::WindowGL2(GraphicsDevice* _Device, WindowSpecifications *_window_spec)
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

WindowGL2::~WindowGL2()
{
	Close();
}

bool WindowGL2::Open()
{
	int t_params = GLFW_WINDOW;

	if (IsOpen())
		return false; // can't open an already open window

	// Enable fullscreen
	if (m_fullscreen)
		t_params = GLFW_FULLSCREEN;

	// Enable vertical sync
	if (m_vsync)
		glfwSwapInterval(1);

	// Set number of FSAA samples
	if (m_fsaa_samples > 0)
		glfwOpenWindowHint(GLFW_FSAA_SAMPLES, m_fsaa_samples);
	
	glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, m_resizeable ? GL_FALSE : GL_TRUE);

	if (GL_TRUE == glfwOpenWindow(m_width, m_height, m_bits_r, m_bits_g, m_bits_b, m_bits_alpha, m_bits_depth, m_bits_stencil, t_params))
	{

		// If we are a window, set title, and position us in the center of the screen
		if (!m_fullscreen)
		{
			SetTitle("OpenGL");

			GLFWvidmode m;
			glfwGetDesktopMode(&m);

			int nx = 0, ny = 0;
			nx = (m.Width / 2) - (m_width / 2);
			ny = (m.Height / 2) - (m_height / 2);
			glfwSetWindowPos(nx, ny);
		}

#ifdef CONF_PLATFORM_MACOSX
		/* HACK - Get events without bundle */
		ProcessSerialNumber psn;	
		GetCurrentProcess(&psn);
		TransformProcessType(&psn,kProcessTransformToForegroundApplication);
		SetFrontProcess(&psn);
#endif
		
		GLenum err = glewInit();
		glewExperimental = true;
		if (err != GLEW_OK)
		{
			GetDevice()->GetKernel()->Log(m_LogTag, "Could not initiate glew.");
		}

		// Map gl-functionality
		Pxf::Graphics::GL::SetupExtensions();

		// Handle window-close gracefully.
		glfwSetWindowCloseCallback(on_window_close);

		GetDevice()->GetKernel()->Log(m_LogTag, "Opened window of %dx%d@%d (r: %d g: %d b: %d a: %d d: %d s: %d)", m_width, m_height, m_bits_r+m_bits_g+m_bits_b+m_bits_alpha, m_bits_r, m_bits_g, m_bits_b, m_bits_alpha, m_bits_depth, m_bits_stencil);

		GetDevice()->GetKernel()->Log(m_LogTag, "OpenGL Vendor  : %s", (const char*)glGetString(GL_VENDOR));
		GetDevice()->GetKernel()->Log(m_LogTag, "OpenGL Renderer: %s", (const char*)glGetString(GL_RENDERER));
		GetDevice()->GetKernel()->Log(m_LogTag, "OpenGL Version : %s", (const char*)glGetString(GL_VERSION));
		GetDevice()->GetKernel()->Log(m_LogTag, "GLSL Version   : %s", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

		return true;
	}
	else
		return false;

	
}

bool WindowGL2::Close()
{
	if(!IsOpen())
		return false;

	glfwCloseWindow();
	return true;
}

void WindowGL2::Swap()
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

		glfwSwapBuffers();
		m_fps_count += 1;
	}
}

bool WindowGL2::IsOpen()
{
	if (glfwGetWindowParam(GLFW_OPENED) == GL_TRUE && !g_ShouldCloseWindow)
		return true;
	else
		return false;
}

bool WindowGL2::IsActive()
{
	if (IsOpen())
	{
		if (GL_TRUE == glfwGetWindowParam(GLFW_ACTIVE))
			return true;
	}
	return false;
}

bool WindowGL2::IsMinimized()
{
	if (IsOpen())
	{
		if (GL_TRUE == glfwGetWindowParam(GLFW_ICONIFIED))
			return true;
	}

	return false;
}

void WindowGL2::SetResizeCallback(WindowSizeCallback fun)
{
	glfwSetWindowSizeCallback(fun);
}

void WindowGL2::SetTitle(const char *_title)
{
	if (IsOpen())
	{
		glfwSetWindowTitle(_title);
	}
}

int WindowGL2::GetFPS()
{
	return m_fps;
}

const char* WindowGL2::GetContextTypeName()
{
	return "OpenGL";
}
