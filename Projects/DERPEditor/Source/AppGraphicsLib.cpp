#include "AppGraphicsLib.h"

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>


int DERPEditor::gfx_redrawneeded (lua_State *L) {
  // gxf.redrawneeded(x,y,w,h) -- x,y,h,w optional, otherwise = full redraw
  if (lua_gettop(L) == 4)
  {
    // TODO: implement sub screen redraw
  } else {
    LuaApp::GetInstance()->m_RedrawNeeded = true;
  }
  return 0;
}

int DERPEditor::gfx_loadtexture (lua_State *L) {
  // new_texture = gfx.loadtexture("file.png")
  if (lua_gettop(L) == 1)
  {
    LuaApp* inst = LuaApp::GetInstance();
    inst->m_QuadBatches[inst->m_QuadBatchCount] = new TexturedQuadBatch(1024,
                                                                        &(inst->m_TransformMatrix),
                                                                        lua_tostring(L, 1));
    lua_pushnumber(L, inst->m_QuadBatchCount); // push qb index as result
    inst->m_QuadBatchCount++;
    return 1;
  } else {
    lua_pushstring(L, "Invalid argument passed to loadtexture function!");
    lua_error(L);
  }
  return 0;
}

int DERPEditor::gfx_bindtexture (lua_State *L) {
  // texture = gfx.bindtexture(textureid)
  if (lua_gettop(L) == 1)
  {
    LuaApp::GetInstance()->ChangeActiveQB(lua_tointeger(L, 1));
    
  } else {
    lua_pushstring(L, "Invalid argument passed to bindtexture function!");
    lua_error(L);
  }
  return 0;
}

int DERPEditor::gfx_translate (lua_State *L) {
  // texture = gfx.translate(x,y)
  if (lua_gettop(L) == 2)
  {
    Pxf::Math::Mat4 t_translatematrix = Pxf::Math::Mat4::Identity;
    t_translatematrix.Translate(lua_tonumber(L, 1), lua_tonumber(L, 2), 0.0f);
    
    LuaApp::GetInstance()->m_TransformMatrix = LuaApp::GetInstance()->m_TransformMatrix * t_translatematrix;
    
  } else {
    lua_pushstring(L, "Invalid argument passed to translate function!");
    lua_error(L);
  }
  return 0;
}

int DERPEditor::gfx_drawcentered (lua_State *L) {
  // texture = gfx.drawcentered(x,y,w,h)
  if (lua_gettop(L) == 4)
  {
    QuadBatch* qb = LuaApp::GetInstance()->GetActiveQB();
    if (qb != NULL)
    {
      qb->ResetTextureSubset();
      qb->AddCentered(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    }
    
  }
  // texture = gfx.drawcentered(x, y, w, h, s0, t0, s1, t1)
  else if (lua_gettop(L) == 8)
  {
    QuadBatch* qb = LuaApp::GetInstance()->GetActiveQB();
    if (qb != NULL)
    {
      qb->SetTextureSubset(lua_tonumber(L, 5), lua_tonumber(L, 6), lua_tonumber(L, 7), lua_tonumber(L, 8));
      qb->AddCentered(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    }
    
  } else {
    lua_pushstring(L, "Invalid argument passed to drawcentered function!");
    lua_error(L);
  }
  return 0;
}


int DERPEditor::luaopen_appgraphics (lua_State *L) {
  const luaL_reg appgraphicslib[] = {
    {"redrawneeded",   gfx_redrawneeded},
    {"loadtexture",   gfx_loadtexture},
    {"bindtexture",   gfx_bindtexture},
    {"translate",   gfx_translate},
    {"drawcentered",   gfx_drawcentered},
    {NULL, NULL}
    };
  
  luaL_register(L, LUA_APPGRAPHICSLIBNAME, appgraphicslib);
  /*lua_pushnumber(L, PI);
  lua_setfield(L, -2, "pi");
  lua_pushnumber(L, HUGE_VAL);
  lua_setfield(L, -2, "huge");*/
  return 1;
}
