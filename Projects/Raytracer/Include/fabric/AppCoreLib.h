#ifndef __FABRIC_APPCORELIB_H__
#define __FABRIC_APPCORELIB_H__

#include "fabric/App.h"

#define LUA_LIB
#include "lua.h"
#include "lauxlib.h"

#define LUA_APPCORENAME "app"

namespace Fabric
{
	// lib functions
	int app_reboot(lua_State *L);
	int app_quit(lua_State *L);
	int app_traceback(lua_State *L);
	int app_getwindimensions(lua_State *L);

	int app_getmemusage(lua_State* L);
	int app_savedialog(lua_State* L);
	int app_opendialog(lua_State* L);
	int app_getcwd(lua_State* L);

	int app__setrenderoption(lua_State *L);
	int app__getrenderoption(lua_State *L);

	int util_hashfile(lua_State *L);
	int util_pathext(lua_State *L);
	int util_pathfilename(lua_State *L);

   // Register lib
   int luaopen_appcore (lua_State *L);
    
    
} /* Fabric */

#endif /* __FABRIC_APPCORELIB_H__ */
