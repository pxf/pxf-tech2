#ifndef _PXF_INPUT_INPUTDEVICE_H_
#define _PXF_INPUT_INPUTDEVICE_H_

#include <Pxf/Base/Types.h>
#include <Pxf/System.h>

#include <Pxf/Input/InputDefs.h>

namespace Pxf {
namespace Input
{
	class InputDevice : public Pxf::System
	{
	public:
	InputDevice(Kernel* _Kernel, const char* _Identifier)
	: Pxf::System(_Kernel, Pxf::System::SYSTEM_TYPE_INPUTDEVICE, _Identifier)
	{}

	virtual ~InputDevice(){};
	
	virtual void Update() = 0;

	virtual bool IsKeyDown(int _key) = 0;
	virtual bool IsButtonDown(int _button) = 0;
	virtual unsigned GetButtonStates() = 0;

	virtual int GetLastKey() = 0;
	virtual int GetLastChar() = 0;
	virtual int GetLastButton() = 0;

	virtual void ClearLastKey() = 0;
	virtual void ClearLastChar() = 0;
	virtual void ClearLastButton() = 0;

	virtual void GetMousePos(int *x, int *y) = 0;
	virtual void SetMousePos(int x, int y) = 0;

	virtual MouseMode GetMouseMode() = 0;
	virtual void SetMouseMode(MouseMode _Mode) = 0;

	virtual void ShowCursor(bool _show) = 0;
	};
}
}

#endif // _PXF_INPUT_INPUTDEVICE_H_