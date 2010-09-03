#ifndef __DERPEDITOR_APPGRAPHICSLIB_H__
#define __DERPEDITOR_APPGRAPHICSLIB_H__

#include "LuaApp.h"

#define LUA_LIB
#include "lua.h"
#include "lauxlib.h"

#define LUA_APPGRAPHICSLIBNAME "gfx"

namespace DERPEditor
{
    // lib functions
    int gfx_test(lua_State *L);
    int gfx_loadtexture(lua_State *L);

    // Register lib
    int luaopen_appgraphics(lua_State *L);
    
    
} /* DERPEditor */

#endif /* __DERPEDITOR_APPGRAPHICSLIB_H__ */