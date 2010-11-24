#ifndef __FABRIC_APPINPUTLIB_H__
#define __FABRIC_APPINPUTLIB_H__

#include "fabric/App.h"

#define LUA_LIB
#include "lua.h"
#include "lauxlib.h"

#define FABRIC_INPUTLIBNAME "inp"

namespace Fabric
{
  
    // lib functions
		int inp_showmouse(lua_State *L);
    int inp_getmousepos(lua_State *L);
    int inp_setmousepos(lua_State *L);
    int inp_isbuttondown(lua_State *L);
    int inp_isbuttonup(lua_State *L);

	  int inp_getlastchar(lua_State *L);
	  int inp_clearlastchar(lua_State *L);
	  
    int inp_iskeydown(lua_State *L);
    int inp_iskeyup(lua_State *L);

    // Register lib
    int luaopen_appinput(lua_State *L);
  
} /* Fabric */

#endif /* __FABRIC_APPINPUTLIB_H__ */