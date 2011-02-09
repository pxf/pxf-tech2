#ifndef __FABRIC_APPSOUNDLIB_H__
#define __FABRIC_APPSOUNDLIB_H__

#include "fabric/App.h"

#define LUA_LIB
#include "lua.h"
#include "lauxlib.h"

#define FABRIC_SOUNDLIBNAME "snd"

namespace Fabric
{
	// lib functions
	int snd_playsound(lua_State *L);
	int snd_stopsound(lua_State *L);
	int snd_stopall(lua_State *L);
	int snd_pauseall(lua_State *L);
	int snd_pausesound(lua_State* L);
	int snd_newsound(lua_State *L);

	// Register lib
	int luaopen_appsound(lua_State *L);
} /* Fabric */

#endif /* __FABRIC_APPSOUNDLIB_H__ */