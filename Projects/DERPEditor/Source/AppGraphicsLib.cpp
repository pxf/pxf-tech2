#include "AppGraphicsLib.h"

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

#include <Pxf/Modules/pri/OpenGL.h>

int DERPEditor::gfx__redrawneeded (lua_State *L) {
  // gxf.redrawneeded(x,y,w,h) -- x,y,h,w optional, otherwise = full redraw
  if (lua_gettop(L) == 4)
  {
    LuaApp::GetInstance()->Redraw(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
  } else {
    LuaApp::GetInstance()->Redraw();
  }
  return 0;
}

int DERPEditor::gfx_loadtexture (lua_State *L) {
  // new_texture = gfx.loadtexture("file.png")
  if (lua_gettop(L) == 1)
  {
    LuaApp* inst = LuaApp::GetInstance();
    inst->m_QuadBatches[inst->m_QuadBatchCount] = new TexturedQuadBatch(LUAAPP_QBSIZE,
                                                                        lua_tostring(L, 1),
                                                                        &(inst->m_CurrentDepth),
                                                                        &(inst->m_CurrentColor),
                                                                        &(inst->m_TransformMatrix)
                                                                        );
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
  // gfx.bindtexture(textureid)
  if (lua_gettop(L) == 1)
  {
    int old = LuaApp::GetInstance()->ChangeActiveQB(lua_tointeger(L, 1));
    lua_pushinteger(L, old);
    return 1;
    
  } else {
    lua_pushstring(L, "Invalid argument passed to bindtexture function!");
    lua_error(L);
  }
  return 0;
}

int DERPEditor::gfx_translate (lua_State *L) {
  // gfx.translate(x,y)
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

int DERPEditor::gfx_rotate (lua_State *L) {
  // gfx.translate(angle)
  if (lua_gettop(L) == 1)
  {
    Pxf::Math::Mat4 t_rotatematrix = Pxf::Math::Mat4::Identity;
    t_rotatematrix.Rotate(lua_tonumber(L, 1), 0.0f,0.0f,1.0f);
    
    LuaApp::GetInstance()->m_TransformMatrix = LuaApp::GetInstance()->m_TransformMatrix * t_rotatematrix;
    
  } else {
    lua_pushstring(L, "Invalid argument passed to translate function!");
    lua_error(L);
  }
  return 0;
}

int DERPEditor::gfx_scale (lua_State *L) {
  // gfx.scale(multiplier)
  if (lua_gettop(L) == 1)
  {
	float _Multiplier = lua_tonumber(L, 1);
	Pxf::Math::Mat4 t_scalematrix = Pxf::Math::Mat4::Identity;
    t_scalematrix.Scale(_Multiplier,_Multiplier,_Multiplier);
    
    LuaApp::GetInstance()->m_TransformMatrix = LuaApp::GetInstance()->m_TransformMatrix * t_scalematrix;
    
  } else {
    lua_pushstring(L, "Invalid argument passed to translate function!");
    lua_error(L);
  }
  return 0;
}

int DERPEditor::gfx_getcolor (lua_State *L) {
  // r,g,b = gfx.getcolor()
  if (lua_gettop(L) == 0)
  {
    lua_pushnumber(L, LuaApp::GetInstance()->m_CurrentColor.r);
    lua_pushnumber(L, LuaApp::GetInstance()->m_CurrentColor.g);
    lua_pushnumber(L, LuaApp::GetInstance()->m_CurrentColor.b);
    return 3;
    
  } else {
    lua_pushstring(L, "Invalid argument passed to getcolor function!");
    lua_error(L);
  }
  return 0;
}

int DERPEditor::gfx_setcolor (lua_State *L) {
  // gfx.setcolor(r,g,b)
  if (lua_gettop(L) == 3)
  {
    LuaApp::GetInstance()->m_CurrentColor = Pxf::Math::Vec4f(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), LuaApp::GetInstance()->m_CurrentColor.a);
    
  } else {
    lua_pushstring(L, "Invalid argument passed to setcolor function!");
    lua_error(L);
  }
  return 0;
}

int DERPEditor::gfx_setclearcolor (lua_State *L) {
  // gfx.setclearcolor(r,g,b)
  if (lua_gettop(L) == 3)
  {
    glClearColor(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), 1.0f);
    
  } else {
    lua_pushstring(L, "Invalid argument passed to setclearcolor function!");
    lua_error(L);
  }
  return 0;
}

int DERPEditor::gfx_drawcentered (lua_State *L) {
  // gfx.drawcentered(x,y,w,h)
  if (lua_gettop(L) == 4)
  {
    QuadBatch* qb = LuaApp::GetInstance()->GetActiveQB();
    if (qb != NULL)
    {
      qb->ResetTextureSubset();
      qb->AddCentered(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
      LuaApp::GetInstance()->IncDepth();
    }
    
  }
  // gfx.drawcentered(x, y, w, h, s0, t0, s1, t1)
  else if (lua_gettop(L) == 8)
  {
    QuadBatch* qb = LuaApp::GetInstance()->GetActiveQB();
    if (qb != NULL)
    {
      qb->SetTextureSubset(lua_tonumber(L, 5), lua_tonumber(L, 6), lua_tonumber(L, 7), lua_tonumber(L, 8));
      qb->AddCentered(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
      LuaApp::GetInstance()->IncDepth();
    }
    
  } else {
    lua_pushstring(L, "Invalid argument passed to drawcentered function!");
    lua_error(L);
  }
  return 0;
}

int DERPEditor::gfx_drawquad (lua_State *L) {
  // gfx.drawquad(x0,y0, x1,y1, x2,y2, x3,y3)
  if (lua_gettop(L) == 8)
  {
    QuadBatch* qb = LuaApp::GetInstance()->GetActiveQB();
    if (qb != NULL)
    {
      qb->ResetTextureSubset();
      qb->AddFreeform(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4),
                      lua_tonumber(L, 5), lua_tonumber(L, 6), lua_tonumber(L, 7), lua_tonumber(L, 8));
      LuaApp::GetInstance()->IncDepth();
    }
    
  }
  // gfx.drawquad(x0,y0, x1,y1, x2,y2, x3,y3, s0, t0, s1, t1)
  else if (lua_gettop(L) == 12)
  {
    QuadBatch* qb = LuaApp::GetInstance()->GetActiveQB();
    if (qb != NULL)
    {
      qb->SetTextureSubset(lua_tonumber(L, 9), lua_tonumber(L, 10), lua_tonumber(L, 11), lua_tonumber(L, 12));
      qb->AddFreeform(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4),
                      lua_tonumber(L, 5), lua_tonumber(L, 6), lua_tonumber(L, 7), lua_tonumber(L, 8));
      LuaApp::GetInstance()->IncDepth();
    }
    
  } else {
    lua_pushstring(L, "Invalid argument passed to drawquad function!");
    lua_error(L);
  }
  return 0;
}

int DERPEditor::gfx_drawtopleft (lua_State *L) {
  // gfx.drawtopleft(x,y,w,h)
  if (lua_gettop(L) == 4)
  {
    QuadBatch* qb = LuaApp::GetInstance()->GetActiveQB();
    if (qb != NULL)
    {
      qb->ResetTextureSubset();
      qb->AddTopLeft(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
      LuaApp::GetInstance()->IncDepth();
    }
    
  }
  // gfx.drawtopleft(x, y, w, h, s0, t0, s1, t1)
  else if (lua_gettop(L) == 8)
  {
    QuadBatch* qb = LuaApp::GetInstance()->GetActiveQB();
    if (qb != NULL)
    {
      qb->SetTextureSubset(lua_tonumber(L, 5), lua_tonumber(L, 6), lua_tonumber(L, 7), lua_tonumber(L, 8));
      qb->AddTopLeft(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
      LuaApp::GetInstance()->IncDepth();
    }
    
  } else {
    lua_pushstring(L, "Invalid argument passed to drawcentered function!");
    lua_error(L);
  }
  return 0;
}


int DERPEditor::luaopen_appgraphics (lua_State *L) {
  const luaL_reg appgraphicslib[] = {
    {"_redrawneeded",   gfx__redrawneeded},
    {"loadtexture",   gfx_loadtexture},
    {"bindtexture",   gfx_bindtexture},
    {"translate",   gfx_translate},
	{"scale",   gfx_scale},
	{"rotate",   gfx_rotate},
    {"getcolor",   gfx_getcolor},
    {"setcolor",   gfx_setcolor},
    {"setclearcolor",   gfx_setclearcolor},
    
    {"drawcentered",   gfx_drawcentered},
    {"drawquad",   gfx_drawquad},
    {"drawtopleft",   gfx_drawtopleft},
    {NULL, NULL}
    };
  
  luaL_register(L, LUA_APPGRAPHICSLIBNAME, appgraphicslib);
  /*lua_pushnumber(L, PI);
  lua_setfield(L, -2, "pi");
  lua_pushnumber(L, HUGE_VAL);
  lua_setfield(L, -2, "huge");*/
  return 1;
}
