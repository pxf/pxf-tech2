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
/*
int inp_isbuttonup(lua_State *L) {
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
}*/

int DERPEditor::luaopen_appinput (lua_State *L) {
  const luaL_reg appinputlib[] = {
    {"getmousepos",   inp_getmousepos},
    {"setmousepos",   inp_getmousepos},
    {"isbuttondown",   inp_isbuttondown},
    {"isbuttonup",   inp_isbuttonup},
    {NULL, NULL}
    };
  
  luaL_register(L, LUA_APPINPUTLIBNAME, appinputlib);
  
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
  return 1;
}
