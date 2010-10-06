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
    int gfx__redrawneeded(lua_State *L);
    int gfx_loadtexture(lua_State *L);
    int gfx_bindtexture(lua_State *L);
    int gfx_translate(lua_State *L);
	  int gfx_scale(lua_State *L);
	  int gfx_rotate(lua_State *L);
    int gfx_getcolor(lua_State *L);
    int gfx_setcolor(lua_State *L);
    int gfx_getalpha(lua_State *L);
    int gfx_setalpha(lua_State *L);
    int gfx_setclearcolor(lua_State *L);

    // special raw texture handling
		int gfx_rawtexture(lua_State *L);
		int gfx_rawtex_delete(lua_State *L);
		int gfx_rawtex_drawquad(lua_State *L);
    
    // draw calls
    int gfx_drawcentered(lua_State *L);
    int gfx_drawquad(lua_State *L);
    int gfx_drawtopleft(lua_State *L);

    // Register lib
    int luaopen_appgraphics(lua_State *L);
    
    
} /* DERPEditor */

#endif /* __DERPEDITOR_APPGRAPHICSLIB_H__ */