#include <Pxf/Pxf.h>
#include <Pxf/Graphics/Window.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Modules/pri/InputDeviceGLFW.h>
#include <Pxf/Util/String.h>

#include <GL/glfw.h>

using namespace Pxf::Modules;

#define LOCAL_MSG "Input"

using namespace Pxf;
using namespace Pxf::Input;
using namespace Pxf::Graphics;
using Util::String;

// -- GLFW callbacks
static void char_callback(int _Char, int _Action)
{
	if (_Action == GLFW_PRESS)
		((InputDeviceGLFW*)Instance)->last_char = _Char;
}

static void key_callback(int _Key, int _Action)
{
	if (_Action == GLFW_PRESS)
		((InputDeviceGLFW*)Instance)->last_key = _Key;
}
static void mouse_callback(int _Button, int _Action)
{
	if (_Action == GLFW_PRESS)
	{
		((InputDeviceGLFW*)Instance)->last_button = _Button + MOUSE_1;

		// set bit to 1
		((InputDeviceGLFW*)Instance)->mouse_buttons |= (1<<_Button);

	}
	else
	{
		// set bit to 0
		((InputDeviceGLFW*)Instance)->mouse_buttons &= ~(1<<_Button);
	}
}
// END -- GLFW callbacks

InputDeviceGLFW::InputDeviceGLFW(Pxf::Kernel* _Kernel)
            : Pxf::Input::InputDevice(_Kernel, "GLFW Input Device")
{
	// Singleton hack for our GLFW callbacks to work
	Instance = this;

	// Reset state vars
	last_char = 0;
	last_key = 0;
	last_button = 0;
	mouse_x = 0;
	mouse_y = 0;
	mouse_scroll = 0;
	mouse_mode = MODE_ABSOLUTE;

	// Setup GLFW callbacks
	glfwSetCharCallback(char_callback);
	glfwSetKeyCallback(key_callback);
	glfwSetMouseButtonCallback(mouse_callback);
}

void InputDeviceGLFW::Update()
{
	glfwPollEvents();
}

bool InputDeviceGLFW::IsKeyDown(int _key)
{
	return glfwGetKey(_key) == GLFW_PRESS;
}

bool InputDeviceGLFW::IsButtonDown(int _button)
{
	return glfwGetMouseButton(_button - MOUSE_1) == GLFW_PRESS;
}

void InputDeviceGLFW::GetMousePos(int *x, int *y)
{
	if (mouse_mode == MODE_RELATIVE)
	{
		int new_x = 0, new_y = 0;
		glfwGetMousePos(&new_x, &new_y);
		*x = new_x - mouse_x;
		*y = new_y - mouse_y;
		mouse_x = new_x;
		mouse_y = new_y;
	}
	else
	{
		glfwGetMousePos(x, y);
	}
}

void InputDeviceGLFW::SetMouseMode(MouseMode _Mode)
{
	if (_Mode == MODE_RELATIVE)
	{
		glfwDisable(GLFW_MOUSE_CURSOR);
		glfwGetMousePos(&mouse_x, &mouse_y);
	}
	else
	{
		glfwEnable(GLFW_MOUSE_CURSOR);
	}

	mouse_mode = _Mode;
}

MouseMode InputDeviceGLFW::GetMouseMode()
{
	return mouse_mode;
}

void InputDeviceGLFW::SetMousePos(int x, int y)
{
	glfwSetMousePos(x, y);
}

void InputDeviceGLFW::ShowCursor(bool _show)
{
	if (_show)
		glfwEnable(GLFW_MOUSE_CURSOR);
	else
		glfwDisable(GLFW_MOUSE_CURSOR);
}

// -- Cached keys, chars and button presses
int InputDeviceGLFW::GetLastKey()
{
	return last_key;
}

int InputDeviceGLFW::GetLastChar()
{
	return last_char;
}

int InputDeviceGLFW::GetLastButton()
{
	return last_button;
}

void InputDeviceGLFW::ClearLastKey()
{
	last_key = 0;
}

void InputDeviceGLFW::ClearLastChar()
{
	last_char = 0;
}

void InputDeviceGLFW::ClearLastButton()
{
	last_button = 0;
}