#ifndef __FABRIC_APPGRAPHICSLIB_H__
#define __FABRIC_APPGRAPHICSLIB_H__

#include "fabric/App.h"

#define LUA_LIB
#include "lua.h"
#include "lauxlib.h"

#define FABRIC_GRAPHICSLIBNAME "gfx"

namespace Fabric
{
	// lib functions
	int gfx__redrawneeded(lua_State *L);
	int gfx_loadidentity(lua_State *L);
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
	int gfx_createshader(lua_State *L);
	int gfx_deleteshader(lua_State *L);
	int gfx_bindshader(lua_State *L);
	int gfx_setuniformf(lua_State *L);

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
} /* Fabric */

#endif /* __FABRIC_APPGRAPHICSLIB_H__ */