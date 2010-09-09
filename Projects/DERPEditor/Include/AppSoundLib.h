#ifndef __DERPEDITOR_APPSOUNDLIB_H__
#define __DERPEDITOR_APPSOUNDLIB_H__

#include "LuaApp.h"

#define LUA_LIB
#include "lua.h"
#include "lauxlib.h"

#define LUA_APPSOUNDLIBNAME "snd"

namespace DERPEditor
{
    // lib functions
    int snd_playsound(lua_State *L);
	int snd_stopsound(lua_State *L);
	int snd_newsound(lua_State *L);

    // Register lib
    int luaopen_appsound(lua_State *L);
} /* DERPEditor */

#endif /* __DERPEDITOR_APPSOUNDLIB_H__ */