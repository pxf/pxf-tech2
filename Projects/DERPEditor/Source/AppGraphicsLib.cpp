#include "AppGraphicsLib.h"


int DERPEditor::gfx_test (lua_State *L) {
  lua_pushnumber(L, 1337);
  return 1;
}

int DERPEditor::gfx_loadtexture (lua_State *L) {
  //printf("im a homo: %s\n", LuaGame::GetInstance()->m_Filepath);
  lua_pushstring(L, LuaApp::GetInstance()->m_Filepath);
  return 1;
}


int DERPEditor::luaopen_appgraphics (lua_State *L) {
  const luaL_reg appgraphicslib[] = {
    {"test",   gfx_test},
    {"loadtexture",   gfx_loadtexture},
    /*{"sin",   math_sin},*/
    {NULL, NULL}
    };
  
  luaL_register(L, LUA_APPGRAPHICSLIBNAME, appgraphicslib);
  /*lua_pushnumber(L, PI);
  lua_setfield(L, -2, "pi");
  lua_pushnumber(L, HUGE_VAL);
  lua_setfield(L, -2, "huge");*/
  return 1;
}
