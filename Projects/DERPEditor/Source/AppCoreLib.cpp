#include "AppCoreLib.h"


int DERPEditor::app_getwindimensions(lua_State *L)
{
  // w,h = app.getwindimensions() -- w,h = width and height of window
  if (lua_gettop(L) == 0)
  {
    Pxf::Graphics::Window* wnd = LuaApp::GetInstance()->m_win;
    lua_pushnumber(L, wnd->GetWidth());
    lua_pushnumber(L, wnd->GetHeight());
    return 2;
    
  } else {
    lua_pushstring(L, "Invalid argument passed to getwindimensions function!");
    lua_error(L);
  }
  return 0;
}

int DERPEditor::app_reboot(lua_State *L)
{
  // app.reboot() -- reboots the application
  if (lua_gettop(L) == 0)
  {
    LuaApp::GetInstance()->Reboot();
    return 0;
    
  } else {
    lua_pushstring(L, "Invalid argument passed to reboot function!");
    lua_error(L);
  }
  return 0;
}

int DERPEditor::app_quit(lua_State *L)
{
  // app.quit() -- quits the application
  if (lua_gettop(L) == 0)
  {
    LuaApp::GetInstance()->Shutdown();
    return 0;
    
  } else {
    lua_pushstring(L, "Invalid argument passed to quit function!");
    lua_error(L);
  }
  return 0;
}

/*
 * Below is a modified version of the debug.traceback function shipped with Lua.
 */

#define LEVELS1	12	/* size of the first part of the stack */
#define LEVELS2	10	/* size of the second part of the stack */

static lua_State *getthread (lua_State *L, int *arg) {
  if (lua_isthread(L, 1)) {
    *arg = 1;
    return lua_tothread(L, 1);
  }
  else {
    *arg = 0;
    return L;
  }
}

int DERPEditor::app_traceback(lua_State *L) {
  int level;
  int firstpart = 1;  /* still before eventual `...' */
  int arg;
  lua_State *L1 = getthread(L, &arg);
  lua_Debug ar;
  if (lua_isnumber(L, arg+2)) {
    level = (int)lua_tointeger(L, arg+2);
    lua_pop(L, 1);
  }
  else
    level = (L == L1) ? 1 : 0;  /* level 0 may be this own function */
  if (lua_gettop(L) == arg)
    lua_pushliteral(L, "");
  else if (!lua_isstring(L, arg+1)) return 1;  /* message is not a string */
  else lua_pushliteral(L, "\n");
  lua_pushliteral(L, "stack traceback:");
  while (lua_getstack(L1, level++, &ar)) {
    if (level > LEVELS1 && firstpart) {
      /* no more than `LEVELS2' more levels? */
      if (!lua_getstack(L1, level+LEVELS2, &ar))
        level--;  /* keep going */
      else {
        lua_pushliteral(L, "\n\t...");  /* too many levels */
        while (lua_getstack(L1, level+LEVELS2, &ar))  /* find last levels */
          level++;
      }
      firstpart = 0;
      continue;
    }
    lua_pushliteral(L, "\n\t");
    lua_getinfo(L1, "Snl", &ar);
    lua_pushfstring(L, "%s:", ar.short_src);
    if (ar.currentline > 0)
      lua_pushfstring(L, "%d:", ar.currentline);
    if (*ar.namewhat != '\0')  /* is there a name? */
        lua_pushfstring(L, " in function " LUA_QS, ar.name);
    else {
      if (*ar.what == 'm')  /* main? */
        lua_pushfstring(L, " in main chunk");
      else if (*ar.what == 'C' || *ar.what == 't')
        lua_pushliteral(L, " ?");  /* C function or tail call */
      else
        lua_pushfstring(L, " in function <%s:%d>",
                           ar.short_src, ar.linedefined);
    }
    lua_concat(L, lua_gettop(L) - arg);
  }
  lua_concat(L, lua_gettop(L) - arg);
  return 1;
}


int DERPEditor::luaopen_appcore (lua_State *L) {
  const luaL_reg appcorelib[] = {
    {"reboot",   app_reboot},
    {"quit",   app_quit},
    {"traceback",   app_traceback},
    {"getwindimensions",   app_getwindimensions},
    {NULL, NULL}
    };
  
  luaL_register(L, LUA_APPCORENAME, appcorelib);
  /*lua_pushnumber(L, PI);
  lua_setfield(L, -2, "pi");
  lua_pushnumber(L, HUGE_VAL);
  lua_setfield(L, -2, "huge");*/
  return 1;
}

