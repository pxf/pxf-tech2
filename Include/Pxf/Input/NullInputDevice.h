#ifndef _PXF_INPUT_NULLINPUTDEVICE_H_
#define _PXF_INPUT_NULLINPUTDEVICE_H_

#include <Pxf/Input/InputDevice.h>
#include <Pxf/Base/Debug.h>

namespace Pxf
{
	namespace Input
	{
		class NullInputDevice : public Pxf::Input::InputDevice
		{
		private:
			virtual bool Init()
			{   return true;	}
		public:
		NullInputDevice(Pxf::Kernel* _Kernel)
			: Pxf::Input::InputDevice(_Kernel, "Null Input Device")
		{}

		virtual ~NullInputDevice(){};
		
		virtual void Update()
		{}

		virtual bool IsKeyDown(int _key)
		{ return false; }
		
		virtual bool IsButtonDown(int _button)
		{ return false; }

		virtual int GetLastKey()
		{ return 0;}
		virtual int GetLastChar()
		{ return 0; }
		virtual int GetLastButton()
		{ return 0; }

		virtual unsigned GetButtonStates()
		{ return 0x0; }

		virtual void ClearLastKey()
		{}
		virtual void ClearLastChar()
		{}
		virtual void ClearLastButton()
		{}

		virtual void GetMousePos(int *x, int *y)
		{}
		virtual void SetMousePos(int x, int y)
		{}

		virtual MouseMode GetMouseMode()
		{
			return MODE_RELATIVE;
		}
		virtual void SetMouseMode(MouseMode _Mode)
		{}

		virtual void ShowCursor(bool _show)
		{}
		};
	}
}

#endif // _PXF_INPUT_NULLINPUTDEVICE_H_