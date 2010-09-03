#include "AppGraphicsLib.h"


int DERPEditor::gfx_test (lua_State *L) {
  lua_pushnumber(L, 1337);
  return 1;
}


const luaL_reg DERPEditor::appgraphicslib[] = {
{"test",   gfx_test},
/*{"cos",   math_cos},
{"sin",   math_sin},*/
{NULL, NULL}
};


int DERPEditor::luaopen_appgraphics (lua_State *L) {
  luaL_register(L, LUA_APPGRAPHICSLIBNAME, appgraphicslib);
  /*lua_pushnumber(L, PI);
  lua_setfield(L, -2, "pi");
  lua_pushnumber(L, HUGE_VAL);
  lua_setfield(L, -2, "huge");*/
  return 1;
}
