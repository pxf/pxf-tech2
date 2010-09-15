#ifndef __DERPEDITOR_APPNETLIB_H__
#define __DERPEDITOR_APPNETLIB_H__

#include "LuaApp.h"

#define LUA_LIB
#include "lua.h"
#include "luaxlib.h"

#define LUA_APPNETLIBNAME "net"

namespace DERPEditor
{
	// lib functions
	int snd_createserver(lua_State *L);

	// Register lib
	int luaopen_appnet(lua_State *L);
} /* DERPEditor */

#endif /* __DERPEDITOR_APPNETLIB_H__ */