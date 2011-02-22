#include <Pxf/Pxf.h>
#include <Pxf/Graphics/Window.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Modules/iphone/InputDeviceIOS.h>
#include <Pxf/Util/String.h>


using namespace Pxf::Modules;
using namespace Pxf;
using namespace Pxf::Input;
using namespace Pxf::Graphics;
using Util::String;

void InputDeviceIOS::mouse_callback(int _Button, int _Action)
{
	int BUTTON_PRESSED = true;
	if (_Action == BUTTON_PRESSED)
	{
		((InputDeviceIOS*)Instance)->last_button = _Button + MOUSE_1;

		// set bit to 1
		((InputDeviceIOS*)Instance)->mouse_buttons |= (1<<_Button);

	}
	else
	{
		// set bit to 0
		((InputDeviceIOS*)Instance)->mouse_buttons &= ~(1<<_Button);
	}
}
// END -- GLFW callbacks

InputDeviceIOS::InputDeviceIOS(Pxf::Kernel* _Kernel)
			: Pxf::Input::InputDevice(_Kernel, "iOS Input Device")
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
	mouse_buttons = 0;

	initialized = false;
}

void InputDeviceIOS::Update()
{
	// Quick fix to make sure this is run after a window has been opened
	// FIXME.
	if (!initialized)
	{
		//glfwEnable(GLFW_KEY_REPEAT);
		//glfwDisable(GLFW_SYSTEM_KEYS);

		// Setup GLFW callbacks
		//glfwSetCharCallback(char_callback);
		//glfwSetKeyCallback(key_callback);
		//glfwSetMouseButtonCallback(mouse_callback);
		initialized = true;
	}

	//glfwPollEvents();
}

bool InputDeviceIOS::IsKeyDown(int _key)
{
	return false;
	//return glfwGetKey(_key) == GLFW_PRESS;
}

bool InputDeviceIOS::IsButtonDown(int _button)
{
	//return glfwGetMouseButton(_button - MOUSE_1) == GLFW_PRESS;
	return false;
}

void InputDeviceIOS::GetMousePos(int *x, int *y)
{
	if (mouse_mode == MODE_RELATIVE)
	{
		int new_x = 0, new_y = 0;
		//glfwGetMousePos(&new_x, &new_y);
		*x = new_x - mouse_x;
		*y = new_y - mouse_y;
		mouse_x = new_x;
		mouse_y = new_y;
	}
	else
	{
		//glfwGetMousePos(x, y);
	}
}

void InputDeviceIOS::SetMouseMode(MouseMode _Mode)
{
	if (_Mode == MODE_RELATIVE)
	{
		//glfwDisable(GLFW_MOUSE_CURSOR);
		//glfwGetMousePos(&mouse_x, &mouse_y);
	}
	else
	{
		//glfwEnable(GLFW_MOUSE_CURSOR);
	}

	mouse_mode = _Mode;
}

MouseMode InputDeviceIOS::GetMouseMode()
{
	return mouse_mode;
}

void InputDeviceIOS::SetMousePos(int x, int y)
{
	//glfwSetMousePos(x, y);
}

void InputDeviceIOS::ShowCursor(bool _show)
{
	//if (_show)
		//glfwEnable(GLFW_MOUSE_CURSOR);
	//else
		//glfwDisable(GLFW_MOUSE_CURSOR);
}

// -- Cached keys, chars and button presses
int InputDeviceIOS::GetLastKey()
{
	return last_key;
}

int InputDeviceIOS::GetLastChar()
{
	return last_char;
}

int InputDeviceIOS::GetLastButton()
{
	return last_button;
}

unsigned InputDeviceIOS::GetButtonStates()
{
	return mouse_buttons;
}

void InputDeviceIOS::ClearLastKey()
{
	last_key = 0;
}

void InputDeviceIOS::ClearLastChar()
{
	last_char = 0;
}

void InputDeviceIOS::ClearLastButton()
{
	last_button = 0;
}