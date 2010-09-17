#ifndef __DERPEDITOR_APPNETLIB_H__
#define __DERPEDITOR_APPNETLIB_H__

#include "LuaApp.h"

#define LUA_LIB
#include "lua.h"
#include "lauxlib.h"

#define LUA_APPNETLIBNAME "net"

#include <Pxf/Network/NetworkDevice.h>

namespace DERPEditor
{
	// lib functions
	int net_createserver(lua_State *L);
	int net_createclient(lua_State *L);

	// server functions
	int net_server_bind(lua_State *L);
	int net_server_delete(lua_State *L);

	// client functions

	// Register lib
	int luaopen_appnet(lua_State *L);
} /* DERPEditor */

#endif /* __DERPEDITOR_APPNETLIB_H__ */