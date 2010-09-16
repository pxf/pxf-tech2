#ifndef __DERPEDITOR_APPINPUTLIB_H__
#define __DERPEDITOR_APPINPUTLIB_H__

#include "LuaApp.h"

#define LUA_LIB
#include "lua.h"
#include "lauxlib.h"

#define LUA_APPINPUTLIBNAME "inp"

namespace DERPEditor
{
  
    // lib functions
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
  
} /* DERPEditor */

#endif /* __DERPEDITOR_APPINPUTLIB_H__ */