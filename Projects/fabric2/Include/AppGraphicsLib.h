#ifndef __DERPEDITOR_APPGRAPHICSLIB_H__
#define __DERPEDITOR_APPGRAPHICSLIB_H__

#include "App.h"

#define LUA_LIB
#include "lua.h"
#include "lauxlib.h"

#define LUA_APPGRAPHICSLIBNAME "gfx"

namespace Fabric
{
	// lib functions
	int gfx_getrawimage (lua_State* L);
	int gfx__redrawneeded(lua_State *L);
	int gfx_loadidentity(lua_State *L);
	
	int gfx_loadtexture(lua_State *L);
	int gfx_newtexture(lua_State *L);
	int gfx_bindtexture(lua_State *L);
	
	int gfx_translate(lua_State *L);
	int gfx_scale(lua_State *L);
	int gfx_rotate(lua_State *L);
	
	// color and alpha
	int gfx_getcolor(lua_State *L);
	int gfx_setcolor(lua_State *L);
	int gfx_getalpha(lua_State *L);
	int gfx_setalpha(lua_State *L);
	
	// modes / settings
	int gfx_clear(lua_State *L);
	int gfx_setview(lua_State *L);
	int gfx_setortho(lua_State *L);
	int gfx_alphatest(lua_State *L);
	int gfx_blending(lua_State *L);
	int gfx_setclearcolor(lua_State *L);
	
	// framebuffers
	int gfx_newframebuffer(lua_State *L);
	int gfx_attachframebuffer(lua_State *L);
	int gfx_detachframebuffer(lua_State *L);
	int gfx_deleteframebuffer(lua_State *L);
	int gfx_bindframebuffer(lua_State *L);
	
	// shaders
	int gfx_createshader(lua_State *L);
	int gfx_unbindshader(lua_State *L);
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
} /* DERPEditor */

#endif /* __DERPEDITOR_APPGRAPHICSLIB_H__ */