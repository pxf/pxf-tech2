#ifndef __DERPEDITOR_APPCORELIB_H__
#define __DERPEDITOR_APPCORELIB_H__

#include "LuaApp.h"

#define LUA_LIB
#include "lua.h"
#include "lauxlib.h"

#define LUA_APPCORENAME "app"

namespace DERPEditor
{
    // lib functions
    int app_traceback(lua_State *L);

    // Register lib
    int luaopen_appcore (lua_State *L);
    
    
} /* DERPEditor */

#endif /* __DERPEDITOR_APPCORELIB_H__ */