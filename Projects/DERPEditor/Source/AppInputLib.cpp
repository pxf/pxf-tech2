#include "AppInputLib.h"

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

#include <Pxf/Input/InputDevice.h>

#include <Pxf/Modules/pri/OpenGL.h>

int DERPEditor::inp_getmousepos (lua_State *L) {
  // x,y = inp.getmousepos() -- x,y mouse position in window
  if (lua_gettop(L) == 0)
  {
    Pxf::Input::InputDevice* inp = Pxf::Kernel::GetInstance()->GetInputDevice();
    int x,y;
    inp->GetMousePos(&x, &y);
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    return 2;
    
  } else {
    lua_pushstring(L, "Invalid argument passed to getmousepos function!");
    lua_error(L);
  }
  return 0;
}

int DERPEditor::inp_setmousepos(lua_State *L) {
  // inp.setmousepos(x,y) -- x,y mouse position in window
  if (lua_gettop(L) == 2)
  {
    Pxf::Input::InputDevice* inp = Pxf::Kernel::GetInstance()->GetInputDevice();
    inp->SetMousePos(lua_tonumber(L, 1), lua_tonumber(L, 2));
    
  } else {
    lua_pushstring(L, "Invalid argument passed to setmousepos function!");
    lua_error(L);
  }
  return 0;
}

int DERPEditor::inp_isbuttondown(lua_State *L) {
  // b = inp.isbuttondown(id) -- b = true if mouse button id is down
  if (lua_gettop(L) == 1)
  {
    Pxf::Input::InputDevice* inp = Pxf::Kernel::GetInstance()->GetInputDevice();
    lua_pushboolean(L, inp->IsButtonDown(lua_tonumber(L, 1)));
    return 1;
    
  } else {
    lua_pushstring(L, "Invalid argument passed to isbuttondown function!");
    lua_error(L);
  }
  return 0;
}

int DERPEditor::inp_isbuttonup(lua_State *L) {
  // b = inp.isbuttonup(id) -- b = true if mouse button id is up
  if (lua_gettop(L) == 1)
  {
    Pxf::Input::InputDevice* inp = Pxf::Kernel::GetInstance()->GetInputDevice();
    lua_pushboolean(L, !(inp->IsButtonDown(lua_tonumber(L, 1))));
    return 1;
    
  } else {
    lua_pushstring(L, "Invalid argument passed to isbuttonup function!");
    lua_error(L);
  }
  return 0;
}

int DERPEditor::inp_iskeydown(lua_State *L) {
  // b = inp.iskeydown(id) -- b = true if keyboard button id is down
  if (lua_gettop(L) == 1)
  {
    Pxf::Input::InputDevice* inp = Pxf::Kernel::GetInstance()->GetInputDevice();
		if (lua_isnumber(L, 1))
			lua_pushboolean(L, inp->IsKeyDown(lua_tonumber(L, 1)));
    else
		{
			const char *tstr = lua_tostring(L, 1);
			lua_pushboolean(L, inp->IsKeyDown((int)(tstr[0])));
		}
    return 1;
    
  } else {
    lua_pushstring(L, "Invalid argument passed to iskeydown function!");
    lua_error(L);
  }
  return 0;
}

int DERPEditor::inp_iskeyup(lua_State *L) {
  // b = inp.iskeyup(id) -- b = true if keyboard button id is up
  if (lua_gettop(L) == 1)
  {
    Pxf::Input::InputDevice* inp = Pxf::Kernel::GetInstance()->GetInputDevice();
    lua_pushboolean(L, !(inp->IsKeyDown(lua_tonumber(L, 1))));
    return 1;
    
  } else {
    lua_pushstring(L, "Invalid argument passed to iskeyup function!");
    lua_error(L);
  }
  return 0;
}

int DERPEditor::inp_getlastchar(lua_State *L)
{
  // c = inp.getlastchar() -- c = last char pressed
  if (lua_gettop(L) == 0)
  {
    Pxf::Input::InputDevice* inp = Pxf::Kernel::GetInstance()->GetInputDevice();
    lua_pushinteger(L, inp->GetLastChar());
    return 1;

  } else {
    lua_pushstring(L, "Invalid argument passed to getlastchar function!");
    lua_error(L);
  }
  return 0;
}

int DERPEditor::inp_clearlastchar(lua_State *L)
{
	// inp.clearlastchar() -- clear last char pressed
  if (lua_gettop(L) == 0)
  {
    Pxf::Kernel::GetInstance()->GetInputDevice()->ClearLastChar();
    return 0;

  } else {
    lua_pushstring(L, "Invalid argument passed to clearlastchar function!");
    lua_error(L);
  }
  return 0;
}


int DERPEditor::luaopen_appinput (lua_State *L) {
  const luaL_reg appinputlib[] = {
    {"getmousepos",   inp_getmousepos},
    {"setmousepos",   inp_getmousepos},
    {"isbuttondown",   inp_isbuttondown},
    {"isbuttonup",   inp_isbuttonup},
    {"getlastchar",   inp_getlastchar},
    {"clearlastchar",   inp_clearlastchar},
    {"iskeydown",   inp_iskeydown},
    {"iskeyup",   inp_iskeyup},
    {NULL, NULL}
    };
  
  luaL_register(L, LUA_APPINPUTLIBNAME, appinputlib);
  
  // mouse inputs
  lua_pushnumber(L, Pxf::Input::MOUSE_1);
  lua_setfield(L, -2, "MOUSE_1");
  lua_pushnumber(L, Pxf::Input::MOUSE_2);
  lua_setfield(L, -2, "MOUSE_2");
  lua_pushnumber(L, Pxf::Input::MOUSE_3);
  lua_setfield(L, -2, "MOUSE_3");
  lua_pushnumber(L, Pxf::Input::MOUSE_4);
  lua_setfield(L, -2, "MOUSE_4");
  lua_pushnumber(L, Pxf::Input::MOUSE_5);
  lua_setfield(L, -2, "MOUSE_5");
  lua_pushnumber(L, Pxf::Input::MOUSE_6);
  lua_setfield(L, -2, "MOUSE_6");
  lua_pushnumber(L, Pxf::Input::MOUSE_7);
  lua_setfield(L, -2, "MOUSE_7");
  lua_pushnumber(L, Pxf::Input::MOUSE_8);
  lua_setfield(L, -2, "MOUSE_8");
  
  lua_pushnumber(L, Pxf::Input::MOUSE_NONE);
  lua_setfield(L, -2, "MOUSE_NONE");
  lua_pushnumber(L, Pxf::Input::MOUSE_LAST);
  lua_setfield(L, -2, "MOUSE_LAST");
  lua_pushnumber(L, Pxf::Input::MOUSE_LEFT);
  lua_setfield(L, -2, "MOUSE_LEFT");
  lua_pushnumber(L, Pxf::Input::MOUSE_RIGHT);
  lua_setfield(L, -2, "MOUSE_RIGHT");
  lua_pushnumber(L, Pxf::Input::MOUSE_MIDDLE);
  lua_setfield(L, -2, "MOUSE_MIDDLE");
  
  // key inputs
  lua_pushnumber(L, Pxf::Input::ESC);
  lua_setfield(L, -2, "ESC");
  lua_pushnumber(L, Pxf::Input::KP_4);
  lua_setfield(L, -2, "KP_4");
  lua_pushnumber(L, Pxf::Input::KP_6);
  lua_setfield(L, -2, "KP_6");
  lua_pushnumber(L, Pxf::Input::LSHIFT);
  lua_setfield(L, -2, "LSHIFT");
  lua_pushnumber(L, Pxf::Input::BACKSPACE);
  lua_setfield(L, -2, "BACKSPACE");
  lua_pushnumber(L, Pxf::Input::F24);
  lua_setfield(L, -2, "F24");
  lua_pushnumber(L, Pxf::Input::KP_0);
  lua_setfield(L, -2, "KP_0");
  lua_pushnumber(L, Pxf::Input::F2);
  lua_setfield(L, -2, "F2");
  lua_pushnumber(L, Pxf::Input::DOWN);
  lua_setfield(L, -2, "DOWN");
  lua_pushnumber(L, Pxf::Input::KP_9);
  lua_setfield(L, -2, "KP_9");
  lua_pushnumber(L, Pxf::Input::F6);
  lua_setfield(L, -2, "F6");
  lua_pushnumber(L, Pxf::Input::INSERT);
  lua_setfield(L, -2, "INSERT");
  lua_pushnumber(L, Pxf::Input::F12);
  lua_setfield(L, -2, "F12");
  lua_pushnumber(L, Pxf::Input::DEL);
  lua_setfield(L, -2, "DEL");
  lua_pushnumber(L, Pxf::Input::F11);
  lua_setfield(L, -2, "F11");
  lua_pushnumber(L, Pxf::Input::F19);
  lua_setfield(L, -2, "F19");
  lua_pushnumber(L, Pxf::Input::F15);
  lua_setfield(L, -2, "F15");
  lua_pushnumber(L, Pxf::Input::F10);
  lua_setfield(L, -2, "F10");
  lua_pushnumber(L, Pxf::Input::ENTER);
  lua_setfield(L, -2, "ENTER");
  lua_pushnumber(L, Pxf::Input::UP);
  lua_setfield(L, -2, "UP");
  lua_pushnumber(L, Pxf::Input::LAST);
  lua_setfield(L, -2, "LAST");
  lua_pushnumber(L, Pxf::Input::F1);
  lua_setfield(L, -2, "F1");
  lua_pushnumber(L, Pxf::Input::F5);
  lua_setfield(L, -2, "F5");
  lua_pushnumber(L, Pxf::Input::F22);
  lua_setfield(L, -2, "F22");
  lua_pushnumber(L, Pxf::Input::SPACE);
  lua_setfield(L, -2, "SPACE");
  lua_pushnumber(L, Pxf::Input::F18);
  lua_setfield(L, -2, "F18");
  lua_pushnumber(L, Pxf::Input::F9);
  lua_setfield(L, -2, "F9");
  lua_pushnumber(L, Pxf::Input::F21);
  lua_setfield(L, -2, "F21");
  lua_pushnumber(L, Pxf::Input::HOME);
  lua_setfield(L, -2, "HOME");
  lua_pushnumber(L, Pxf::Input::F25);
  lua_setfield(L, -2, "F25");
  lua_pushnumber(L, Pxf::Input::F20);
  lua_setfield(L, -2, "F20");
  lua_pushnumber(L, Pxf::Input::PAGEDOWN);
  lua_setfield(L, -2, "PAGEDOWN");
  lua_pushnumber(L, Pxf::Input::F13);
  lua_setfield(L, -2, "F13");
  lua_pushnumber(L, Pxf::Input::F17);
  lua_setfield(L, -2, "F17");
  lua_pushnumber(L, Pxf::Input::F4);
  lua_setfield(L, -2, "F4");
  lua_pushnumber(L, Pxf::Input::UNKNOWN);
  lua_setfield(L, -2, "UNKNOWN");
  lua_pushnumber(L, Pxf::Input::SPECIAL);
  lua_setfield(L, -2, "SPECIAL");
  lua_pushnumber(L, Pxf::Input::F8);
  lua_setfield(L, -2, "F8");
  lua_pushnumber(L, Pxf::Input::LALT);
  lua_setfield(L, -2, "LALT");
  lua_pushnumber(L, Pxf::Input::KP_ENTER);
  lua_setfield(L, -2, "KP_ENTER");
  lua_pushnumber(L, Pxf::Input::KP_1);
  lua_setfield(L, -2, "KP_1");
  lua_pushnumber(L, Pxf::Input::KP_DECIMAL);
  lua_setfield(L, -2, "KP_DECIMAL");
  lua_pushnumber(L, Pxf::Input::KP_7);
  lua_setfield(L, -2, "KP_7");
  lua_pushnumber(L, Pxf::Input::KP_SUBTRACT);
  lua_setfield(L, -2, "KP_SUBTRACT");
  lua_pushnumber(L, Pxf::Input::END);
  lua_setfield(L, -2, "END");
  lua_pushnumber(L, Pxf::Input::KP_DIVIDE);
  lua_setfield(L, -2, "KP_DIVIDE");
  lua_pushnumber(L, Pxf::Input::TAB);
  lua_setfield(L, -2, "TAB");
  lua_pushnumber(L, Pxf::Input::RCTRL);
  lua_setfield(L, -2, "RCTRL");
  lua_pushnumber(L, Pxf::Input::KP_ADD);
  lua_setfield(L, -2, "KP_ADD");
  lua_pushnumber(L, Pxf::Input::F14);
  lua_setfield(L, -2, "F14");
  lua_pushnumber(L, Pxf::Input::KP_5);
  lua_setfield(L, -2, "KP_5");
  lua_pushnumber(L, Pxf::Input::F23);
  lua_setfield(L, -2, "F23");
  lua_pushnumber(L, Pxf::Input::KP_3);
  lua_setfield(L, -2, "KP_3");
  lua_pushnumber(L, Pxf::Input::F3);
  lua_setfield(L, -2, "F3");
  lua_pushnumber(L, Pxf::Input::KP_2);
  lua_setfield(L, -2, "KP_2");
  lua_pushnumber(L, Pxf::Input::KP_EQUAL);
  lua_setfield(L, -2, "KP_EQUAL");
  lua_pushnumber(L, Pxf::Input::F7);
  lua_setfield(L, -2, "F7");
  lua_pushnumber(L, Pxf::Input::LCTRL);
  lua_setfield(L, -2, "LCTRL");
  lua_pushnumber(L, Pxf::Input::KP_MULTIPLY);
  lua_setfield(L, -2, "KP_MULTIPLY");
  lua_pushnumber(L, Pxf::Input::RALT);
  lua_setfield(L, -2, "RALT");
  lua_pushnumber(L, Pxf::Input::RIGHT);
  lua_setfield(L, -2, "RIGHT");
  lua_pushnumber(L, Pxf::Input::KP_8);
  lua_setfield(L, -2, "KP_8");
  lua_pushnumber(L, Pxf::Input::F16);
  lua_setfield(L, -2, "F16");
  lua_pushnumber(L, Pxf::Input::PAGEUP);
  lua_setfield(L, -2, "PAGEUP");
  lua_pushnumber(L, Pxf::Input::LEFT);
  lua_setfield(L, -2, "LEFT");
  lua_pushnumber(L, Pxf::Input::RSHIFT);
  lua_setfield(L, -2, "RSHIFT");	
  
  
  
  return 1;
}
