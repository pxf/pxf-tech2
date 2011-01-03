#include "AppGraphicsLib.h"

#include <Pxf/Kernel.h>
#include <Pxf/Base/Stream.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Modules/pri/OpenGL.h>

using namespace Pxf;

int Fabric::gfx__redrawneeded (lua_State *L) {
  // gxf.redrawneeded(x,y,w,h) -- x,y,h,w optional, otherwise = full redraw
  if (lua_gettop(L) == 4)
  {
    App::GetInstance()->Redraw(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
  } else {
    App::GetInstance()->Redraw();
  }
  return 0;
}

int Fabric::gfx_loadtexture (lua_State *L) {
  // new_texture = gfx.loadtexture(123, "file.png") -- 123 = max quads count
  if (lua_gettop(L) == 2 || lua_gettop(L) == 3)
  {
		bool tlinear = false;
		if (lua_gettop(L) == 3)
		{
			tlinear = lua_toboolean(L, 3);
		}
	
    App* inst = App::GetInstance();
    int quadcount = lua_tointeger(L, 1);
    inst->m_MaxQuadCount += quadcount;
    inst->m_DepthStep = (FABRIC_DEPTH_RANGE / inst->m_MaxQuadCount);
    inst->m_QuadBatches[inst->m_QuadBatchCount] = new TexturedQuadBatch(quadcount*4,
                                                                        lua_tostring(L, 2),
                                                                        &(inst->m_CurrentDepth),
                                                                        &(inst->m_CurrentColor),
                                                                        &(inst->m_TransformMatrix),
                                                                        tlinear);
    lua_pushnumber(L, inst->m_QuadBatchCount); // push qb index as result
    inst->m_QuadBatchCount++;
    return 1;
  } else {
    lua_pushstring(L, "Invalid argument passed to loadtexture function!");
    lua_error(L);
  }
  return 0;
}

int Fabric::gfx_loadidentity (lua_State *L) {
  // gfx.loadidentity() -- reset matrix
  if (lua_gettop(L) == 0)
  {
    App* inst = App::GetInstance();
		inst->m_TransformMatrix = Pxf::Math::Mat4::Identity;
    return 0;
  } else {
    lua_pushstring(L, "Invalid argument passed to loadidentity function!");
    lua_error(L);
  }
  return 0;
}

int Fabric::gfx_rawtexture (lua_State *L) {
  // new_raw_texture = gfx.rawtexture(quad_number, width, height, channels, data)
  if (lua_gettop(L) == 5)
  {
	
		// Get char-pointer
		lua_getfield(L, 5, "instance");
		const unsigned char** t_data = (const unsigned char**)lua_touserdata(L, -1);
		
		// Setup table to return
		lua_newtable(L);
		TexturedQuadBatch** new_raw_tex = (TexturedQuadBatch**)lua_newuserdata(L, sizeof(TexturedQuadBatch*));
	
		// Create quadbatch
    App* inst = App::GetInstance();
    int quadcount = lua_tointeger(L, 1);
    inst->m_MaxQuadCount += quadcount;
    inst->m_DepthStep = (FABRIC_DEPTH_RANGE / inst->m_MaxQuadCount);
    *new_raw_tex = new TexturedQuadBatch(quadcount*4,
                                         lua_tonumber(L, 2),
                                         lua_tonumber(L, 3),
                                         lua_tonumber(L, 4),
                                         *t_data,
                                         &(inst->m_CurrentDepth),
                                         &(inst->m_CurrentColor),
                                         &(inst->m_TransformMatrix)
                                        );
		
		// Push it to the apps (special raw texture) qb array
		inst->m_RawTextureQB.push_back(*new_raw_tex);
		
		// Set metatable
		luaL_getmetatable(L, "gfx.rawtex");
		lua_setmetatable(L, -2);
		
		// Set member functions
		lua_setfield(L, -2, "instance");
		lua_pushcfunction(L, gfx_rawtex_drawquad);
		lua_setfield(L, -2, "draw");
		
    return 1;
  } else {
    lua_pushstring(L, "Invalid argument passed to rawtexture function!");
    lua_error(L);
  }
  return 0;
}

int Fabric::gfx_rawtex_delete(lua_State *L)
{
	if (lua_gettop(L) == 1)
	{
		// TODO: Remove shieeeeeeeeeeeet
		//printf("IMMA BIN REMOVIN\n");
		TexturedQuadBatch** tqb = (TexturedQuadBatch**)lua_touserdata(L, 1);
		App* inst = App::GetInstance();
		Util::Array<TexturedQuadBatch*>::iterator iter;
		for(iter = inst->m_RawTextureQB.begin(); iter != inst->m_RawTextureQB.end(); ++iter)
		{
			if (*iter == *tqb)
			{
				break;
			}
		}
		inst->m_RawTextureQB.erase(iter);
		delete *tqb;
		
		return 0;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to delete function! (of raw texture)");
		lua_error(L);
	}

	return 0;
}

int Fabric::gfx_rawtex_drawquad(lua_State *L)
{
  // raw_tex:draw(x0,y0, x1,y1, x2,y2, x3,y3)
  if (lua_gettop(L) == 9)
  {
		lua_getfield(L, 1, "instance");
		TexturedQuadBatch* qb = *(TexturedQuadBatch**)lua_touserdata(L, -1);
    if (qb != NULL)
    {
			qb->Begin();
      qb->ResetTextureSubset();
      qb->AddFreeform(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5),
                      lua_tonumber(L, 6), lua_tonumber(L, 7), lua_tonumber(L, 8), lua_tonumber(L, 9));
			qb->End();
      App::GetInstance()->IncDepth();
    }

  }
  // raw_tex:draw(x0,y0, x1,y1, x2,y2, x3,y3, s0, t0, s1, t1)
  else if (lua_gettop(L) == 13)
  {
    lua_getfield(L, 1, "instance");
    TexturedQuadBatch* qb = *(TexturedQuadBatch**)lua_touserdata(L, -1);
		if (qb != NULL)
    {
			qb->Begin();
      qb->SetTextureSubset(lua_tonumber(L, 10), lua_tonumber(L, 11), lua_tonumber(L, 12), lua_tonumber(L, 13));
      qb->AddFreeform(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5),
                      lua_tonumber(L, 6), lua_tonumber(L, 7), lua_tonumber(L, 8), lua_tonumber(L, 9));
			qb->End();
      App::GetInstance()->IncDepth();
    }

  } else {
    lua_pushstring(L, "Invalid argument passed to draw function! (of raw texture)");
    lua_error(L);
  }
  return 0;
}

int Fabric::gfx_bindtexture (lua_State *L) {
  // gfx.bindtexture(textureid)
  if (lua_gettop(L) == 1)
  {
    int old = App::GetInstance()->ChangeActiveQB(lua_tointeger(L, 1));
    lua_pushinteger(L, old);
    return 1;
    
  } else {
    lua_pushstring(L, "Invalid argument passed to bindtexture function!");
    lua_error(L);
  }
  return 0;
}

int Fabric::gfx_translate (lua_State *L) {
  // gfx.translate(x,y)
  if (lua_gettop(L) == 2)
  {
    Pxf::Math::Mat4 t_translatematrix = Pxf::Math::Mat4::Identity;
    t_translatematrix.Translate(lua_tonumber(L, 1), lua_tonumber(L, 2), 0.0f);
    
    App::GetInstance()->m_TransformMatrix = App::GetInstance()->m_TransformMatrix * t_translatematrix;
    
  } else {
    lua_pushstring(L, "Invalid argument passed to translate function!");
    lua_error(L);
  }
  return 0;
}

int Fabric::gfx_rotate (lua_State *L) {
  // gfx.translate(angle)
  if (lua_gettop(L) == 1)
  {
    Pxf::Math::Mat4 t_rotatematrix = Pxf::Math::Mat4::Identity;
    t_rotatematrix.Rotate(lua_tonumber(L, 1), 0.0f,0.0f,1.0f);
    
    App::GetInstance()->m_TransformMatrix = App::GetInstance()->m_TransformMatrix * t_rotatematrix;
    
  } else {
    lua_pushstring(L, "Invalid argument passed to translate function!");
    lua_error(L);
  }
  return 0;
}

int Fabric::gfx_scale (lua_State *L) {
  // gfx.scale(multiplier)
  if (lua_gettop(L) == 1)
  {
	float _Multiplier = lua_tonumber(L, 1);
	Pxf::Math::Mat4 t_scalematrix = Pxf::Math::Mat4::Identity;
    t_scalematrix.Scale(_Multiplier,_Multiplier,_Multiplier);
    
    App::GetInstance()->m_TransformMatrix = App::GetInstance()->m_TransformMatrix * t_scalematrix;
    
  } else {
    lua_pushstring(L, "Invalid argument passed to translate function!");
    lua_error(L);
  }
  return 0;
}

int Fabric::gfx_getcolor (lua_State *L) {
  // r,g,b = gfx.getcolor()
  if (lua_gettop(L) == 0)
  {
    lua_pushnumber(L, App::GetInstance()->m_CurrentColor.r);
    lua_pushnumber(L, App::GetInstance()->m_CurrentColor.g);
    lua_pushnumber(L, App::GetInstance()->m_CurrentColor.b);
    return 3;
    
  } else {
    lua_pushstring(L, "Invalid argument passed to getcolor function!");
    lua_error(L);
  }
  return 0;
}

int Fabric::gfx_setcolor (lua_State *L) {
  // gfx.setcolor(r,g,b)
  if (lua_gettop(L) == 3)
  {
    App::GetInstance()->m_CurrentColor = Pxf::Math::Vec4f(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), App::GetInstance()->m_CurrentColor.a);
    
  } else {
    lua_pushstring(L, "Invalid argument passed to setcolor function!");
    lua_error(L);
  }
  return 0;
}

int Fabric::gfx_getalpha (lua_State *L) {
  // a = gfx.getalpha()
  if (lua_gettop(L) == 0)
  {
    lua_pushnumber(L, App::GetInstance()->m_CurrentColor.a);
    return 1;
    
  } else {
    lua_pushstring(L, "Invalid argument passed to getalpha function!");
    lua_error(L);
  }
  return 0;
}

int Fabric::gfx_setalpha (lua_State *L) {
  // gfx.setalpha(a)
  if (lua_gettop(L) == 1)
  {
    App::GetInstance()->m_CurrentColor.a = lua_tonumber(L, 1);
    
  } else {
    lua_pushstring(L, "Invalid argument passed to setalpha function!");
    lua_error(L);
  }
  return 0;
}

int Fabric::gfx_setclearcolor (lua_State *L) {
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

int Fabric::gfx_drawcentered (lua_State *L) {
  // gfx.drawcentered(x,y,w,h)
  if (lua_gettop(L) == 4)
  {
		App::GetInstance()->SetDirty();
    QuadBatch* qb = App::GetInstance()->GetActiveQB();
    if (qb != NULL)
    {
      qb->ResetTextureSubset();
      qb->AddCentered(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
      App::GetInstance()->IncDepth();
    }
    
  }
  // gfx.drawcentered(x, y, w, h, s0, t0, s1, t1)
  else if (lua_gettop(L) == 8)
  {
		App::GetInstance()->SetDirty();
    QuadBatch* qb = App::GetInstance()->GetActiveQB();
    if (qb != NULL)
    {
      qb->SetTextureSubset(lua_tonumber(L, 5), lua_tonumber(L, 6), lua_tonumber(L, 7), lua_tonumber(L, 8));
      qb->AddCentered(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
      App::GetInstance()->IncDepth();
    }
    
  } else {
    lua_pushstring(L, "Invalid argument passed to drawcentered function!");
    lua_error(L);
  }
  return 0;
}

int Fabric::gfx_drawquad (lua_State *L) {
  // gfx.drawquad(x0,y0, x1,y1, x2,y2, x3,y3)
  if (lua_gettop(L) == 8)
  {
		App::GetInstance()->SetDirty();
    QuadBatch* qb = App::GetInstance()->GetActiveQB();
    if (qb != NULL)
    {
      qb->ResetTextureSubset();
      qb->AddFreeform(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4),
                      lua_tonumber(L, 5), lua_tonumber(L, 6), lua_tonumber(L, 7), lua_tonumber(L, 8));
      App::GetInstance()->IncDepth();
    }
    
  }
  // gfx.drawquad(x0,y0, x1,y1, x2,y2, x3,y3, s0, t0, s1, t1)
  else if (lua_gettop(L) == 12)
  {
		App::GetInstance()->SetDirty();
    QuadBatch* qb = App::GetInstance()->GetActiveQB();
    if (qb != NULL)
    {
      qb->SetTextureSubset(lua_tonumber(L, 9), lua_tonumber(L, 10), lua_tonumber(L, 11), lua_tonumber(L, 12));
      qb->AddFreeform(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4),
                      lua_tonumber(L, 5), lua_tonumber(L, 6), lua_tonumber(L, 7), lua_tonumber(L, 8));
      App::GetInstance()->IncDepth();
    }
    
  } else {
    lua_pushstring(L, "Invalid argument passed to drawquad function!");
    lua_error(L);
  }
  return 0;
}

int Fabric::gfx_drawtopleft (lua_State *L) {
  // gfx.drawtopleft(x,y,w,h)
  if (lua_gettop(L) == 4)
  {
		App::GetInstance()->SetDirty();
    QuadBatch* qb = App::GetInstance()->GetActiveQB();
    if (qb != NULL)
    {
      qb->ResetTextureSubset();
      qb->AddTopLeft(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
      App::GetInstance()->IncDepth();
    }
    
  }
  // gfx.drawtopleft(x, y, w, h, s0, t0, s1, t1)
  else if (lua_gettop(L) == 8)
  {
		App::GetInstance()->SetDirty();
    QuadBatch* qb = App::GetInstance()->GetActiveQB();
    if (qb != NULL)
    {
      qb->SetTextureSubset(lua_tonumber(L, 5), lua_tonumber(L, 6), lua_tonumber(L, 7), lua_tonumber(L, 8));
      qb->AddTopLeft(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
      App::GetInstance()->IncDepth();
    }
    
  } else {
    lua_pushstring(L, "Invalid argument passed to drawcentered function!");
    lua_error(L);
  }
  return 0;
}

int Fabric::gfx_createshader(lua_State *L)
{
	if (lua_gettop(L) == 3)
	{
		lua_newtable(L);
		Pxf::Graphics::Shader** shdr = (Pxf::Graphics::Shader**)lua_newuserdata(L, sizeof(Pxf::Graphics::Shader*));

		const char* ident = lua_tostring(L, 1);
		const char* vshader = lua_tostring(L, 2);
		const char* fshader = lua_tostring(L, 3);
		*shdr = App::GetInstance()->m_gfx->CreateShader(ident, vshader, fshader);
		
		// Set metatable
		luaL_getmetatable(L, "gfx.shader");
		lua_setmetatable(L, -2);

		// Set member functions
		lua_setfield(L, -2, "instance");
		lua_pushcfunction(L, gfx_bindshader);
		lua_setfield(L, -2, "bind");
		lua_pushcfunction(L, gfx_setuniformf);
		lua_setfield(L, -2, "setuniformf");

		return 1;
	}
	else
	{
		lua_pushstring(L, "Invalid argument passed to createshader function!");
		lua_error(L);
	}
	return 0;
}

int Fabric::gfx_bindshader(lua_State *L)
{
	if (lua_gettop(L) == 1)
	{
		lua_getfield(L, 1, "instance");
		Graphics::Shader* shdr = *(Graphics::Shader**)lua_touserdata(L, -1);
		//Graphics::Shader* old = App::GetInstance()->m_gfx->BindShader(shdr); // fix l8r
		return 0;
	}
	else
	{
		lua_pushstring(L, "Invalid argument passed to bindshader function!");
		lua_error(L);
	}
	return 0;
}

int Fabric::gfx_setuniformf(lua_State *L)
{
	if (lua_gettop(L) == 3)
	{
		lua_getfield(L, 1, "instance");
		Graphics::Shader* shdr = *(Graphics::Shader**)lua_touserdata(L, -1);
		const char* name = lua_tostring(L, 2);
		float value = (float)lua_tonumber(L, 3);
		App::GetInstance()->m_gfx->SetUniformf(shdr, name, value);
		return 0;
	}
	else
	{
		lua_pushstring(L, "Invalid argument passed to setuniformf function!");
		lua_error(L);
	}
	return 0;
}

int Fabric::gfx_deleteshader(lua_State *L)
{
	if (lua_gettop(L) == 1)
	{
		// TODO: Remove shieeeeeeeeeeeet
		printf("--- TODO: IMMA BIN REMOVIN ZE SHADER\n");
		return 0;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to delete function! (of shader)");
		lua_error(L);
	}

	return 0;
}


int Fabric::luaopen_appgraphics (lua_State *L) {
  const luaL_reg appgraphicslib[] = {
    {"_redrawneeded",   gfx__redrawneeded},
    {"loadidentity",   gfx_loadidentity},
    {"loadtexture",   gfx_loadtexture},
    {"rawtexture",   gfx_rawtexture},
    {"bindtexture",   gfx_bindtexture},
    {"translate",   gfx_translate},
  	{"scale",   gfx_scale},
  	{"rotate",   gfx_rotate},
    {"getcolor",   gfx_getcolor},
    {"setcolor",   gfx_setcolor},
    {"getalpha",   gfx_getalpha},
    {"setalpha",   gfx_setalpha},
    {"setclearcolor",   gfx_setclearcolor},

	{"createshader",   gfx_createshader},
    
    {"drawcentered",   gfx_drawcentered},
    {"drawquad",   gfx_drawquad},
    {"drawtopleft",   gfx_drawtopleft},
    {NULL, NULL}
    };
  
  luaL_register(L, LUA_APPGRAPHICSLIBNAME, appgraphicslib);

	// set __gc for gfx.rawtex
	luaL_newmetatable(L, "gfx.rawtex");
	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, gfx_rawtex_delete);
	lua_settable(L, -3);
	
	// set __gc for gfx.shader
	luaL_newmetatable(L, "gfx.shader");
	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, gfx_deleteshader);
	lua_settable(L, -3);

  /*lua_pushnumber(L, PI);
  lua_setfield(L, -2, "pi");
  lua_pushnumber(L, HUGE_VAL);
  lua_setfield(L, -2, "huge");*/
  return 1;
}
