#ifndef _PXF_MODULES_PRI_INPUTDEVICEGLFW_H_
#define _PXF_MODULES_PRI_INPUTDEVICEGLFW_H_

#include <Pxf/Input/InputDevice.h>
#include <Pxf/Input/InputDefs.h>
#include <Pxf/Base/Debug.h>

namespace Pxf
{
	namespace Input
	{
		class InputDevice;
	}
    namespace Modules
    {
		static Input::InputDevice* Instance;

        class InputDeviceGLFW : public Pxf::Input::InputDevice
        {
        private:
            virtual bool Init()
            {   return true;    }
        public:

		static void char_callback(int _Char, int _Action);
		static void key_callback(int _Key, int _Action);
		static void mouse_callback(int _Button, int _Action);

        InputDeviceGLFW(Pxf::Kernel* _Kernel);
		virtual ~InputDeviceGLFW(){};

        virtual void Update();

        virtual bool IsKeyDown(int _key);
        
        virtual bool IsButtonDown(int _button);

        virtual int GetLastKey();
        virtual int GetLastChar();
        virtual int GetLastButton();
        virtual void ClearLastKey();
        virtual void ClearLastChar();
        virtual void ClearLastButton();

        virtual void GetMousePos(int *x, int *y);
        virtual void SetMousePos(int x, int y);
        virtual Input::MouseMode GetMouseMode();
        virtual void SetMouseMode(Input::MouseMode _Mode);
        virtual void ShowCursor(bool _show);

		// State vars
		int last_char;   /* stores last char pressed */
		int last_key;    /* stores last key pressed (redundant? glfw :E) */
		int last_button; /* stores last mouse button pressed */

		Input::MouseMode mouse_mode;
		int mouse_x, mouse_y, mouse_scroll;
		unsigned int mouse_buttons; /* store status for mouse button, bit #0 = button 1. 1 = pressed */ 
		bool initialized;
        };
    }
}

#endif // _PXF_MODULES_PRI_INPUTDEVICEGLFW_H_