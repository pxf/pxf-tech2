#include "AppGraphicsLib.h"

#include <Pxf/Kernel.h>
#include <Pxf/Base/Stream.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Graphics/FrameBufferObject.h>
#include <Pxf/Modules/pri/OpenGL.h>
#include <Pxf/Resource/Image.h>
#include <Pxf/Resource/ResourceManager.h>

using namespace Pxf;

int Fabric::gfx_getrawimage (lua_State* L) {
	// gfx.getrawimage(filepath)
	if (lua_gettop(L) == 1)
	{
		Resource::ResourceManager* res = Kernel::GetInstance()->GetResourceManager();
		Resource::Image* img = res->Acquire<Resource::Image>(lua_tostring(L, 1));
		lua_createtable(L, img->Width()*img->Height()*img->Channels(), 0);
		for(int y = 0; y < img->Height(); ++y)
		{
			for(int x = 0; x < img->Width(); ++x)
			{
				for(int c = 0; c < img->Channels(); ++c)
				{
					lua_pushnumber(L, y*img->Width()*img->Channels()+x*img->Channels()+c);
					lua_pushnumber(L, img->Ptr()[y*img->Width()*img->Channels()+x*img->Channels()+c]);
					lua_settable(L, -3);
				}
			}
		}
	
		return 1;
	} else {
    lua_pushstring(L, "Invalid argument passed to getrawimage function!");
    lua_error(L);
  }
  return 0;
}

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
  // new_texture = gfx.loadtexture(123, "file.png", linear = false) -- 123 = max quads count
  if (lua_gettop(L) == 2 || lua_gettop(L) == 3)
  {
		bool tlinear = false;
		if (lua_gettop(L) == 3)
		{
			tlinear = lua_toboolean(L, 3);
			printf("should be linear %d\n", tlinear);
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

int Fabric::gfx_newtexture (lua_State *L) {
  // new_texture = gfx.newtexture(quadscount, w, h, [linear]) -- w/h - dimensions, format = RGBA8 etc
  if (lua_gettop(L) == 3 || lua_gettop(L) == 4)
  {
		bool tlinear = false;
		if (lua_gettop(L) == 4)
		{
			tlinear = lua_toboolean(L, 4);
		}
		
		// Create empty texture
		Graphics::Texture* tex = App::GetInstance()->m_gfx->CreateEmptyTexture(lua_tonumber(L, 2), lua_tonumber(L, 3));
		
	
    App* inst = App::GetInstance();
    int quadcount = lua_tointeger(L, 1);
    inst->m_MaxQuadCount += quadcount;
    inst->m_DepthStep = (FABRIC_DEPTH_RANGE / inst->m_MaxQuadCount);
    inst->m_QuadBatches[inst->m_QuadBatchCount] = new TexturedQuadBatch(quadcount*4,
                                                                        lua_tonumber(L, 2),
																																				lua_tonumber(L, 3),
																																				4,
																																				tex,
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

int Fabric::gfx_blending (lua_State *L) {
  // gfx.blending(sfactor, dfactor) -- specify pixel arithmetic (no params = disable blending)
  if (lua_gettop(L) == 2)
  {
		App::GetInstance()->Flush();
		
		glEnable(GL_BLEND);
		glBlendFunc(lua_tonumber(L, 1), lua_tonumber(L, 2));
    
  } else if (lua_gettop(L) == 0)
	{
			App::GetInstance()->Flush();
			glDisable(GL_BLEND);

	 } else {
    lua_pushstring(L, "Invalid argument passed to setalpha function!");
    lua_error(L);
  }
  return 0;
}

int Fabric::gfx_alphatest (lua_State *L) {
  // gfx.alphatest(func, ref) -- specify the alpha test function (no params = disable alpha test)
  if (lua_gettop(L) == 2)
  {
		App::GetInstance()->Flush();
		
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(lua_tonumber(L, 1), lua_tonumber(L, 2));
    
  } else if (lua_gettop(L) == 0)
	{
			App::GetInstance()->Flush();
			glDisable(GL_ALPHA_TEST);

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

int Fabric::gfx_clear (lua_State *L) {
  // gfx.clear()
  if (lua_gettop(L) == 0)
  {
		//glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
  } else {
    lua_pushstring(L, "Invalid argument passed to clear function!");
    lua_error(L);
  }
  return 0;
}

int Fabric::gfx_setortho (lua_State *L) {
  // gfx.setview(x,y,w,h)
  if (lua_gettop(L) == 4)
  {
    Math::Mat4 prjmat = Math::Mat4::Ortho(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4), FABRIC_DEPTH_FAR, FABRIC_DEPTH_NEAR);
		App::GetInstance()->m_gfx->SetProjection(&prjmat);
    
  } else {
    lua_pushstring(L, "Invalid argument passed to setview function!");
    lua_error(L);
  }
  return 0;
}

int Fabric::gfx_setview (lua_State *L) {
  // gfx.setortho(x,y,w,h)
  if (lua_gettop(L) == 4)
  {
		App::GetInstance()->m_gfx->SetViewport(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
    
  } else {
    lua_pushstring(L, "Invalid argument passed to setortho function!");
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
		//lua_pushcfunction(L, gfx_bindshader);
		//lua_setfield(L, -2, "bind");
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
	if (lua_gettop(L) == 0)
	{
		App::GetInstance()->Flush();
		Graphics::Shader* old = App::GetInstance()->m_gfx->BindShader(0);
		return 0;
		
	} else if (lua_gettop(L) == 1)
	{
		App::GetInstance()->Flush();
		lua_getfield(L, 1, "instance");
		Graphics::Shader* shdr = *(Graphics::Shader**)lua_touserdata(L, -1);
		Graphics::Shader* old = App::GetInstance()->m_gfx->BindShader(shdr);
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
		App::GetInstance()->m_gfx->DestroyShader(*(Graphics::Shader**)lua_touserdata(L, 1));
		return 0;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to delete function! (of shader)");
		lua_error(L);
	}

	return 0;
}

int Fabric::gfx_newframebuffer(lua_State *L)
{
	if (lua_gettop(L) == 0)
	{
		lua_newtable(L);
		Pxf::Graphics::FrameBufferObject** fbo = (Pxf::Graphics::FrameBufferObject**)lua_newuserdata(L, sizeof(Pxf::Graphics::FrameBufferObject*));

		//int width = lua_tonumber(L, 1);
		//int height = lua_tonumber(L, 2);
		*fbo = App::GetInstance()->m_gfx->CreateFrameBufferObject();
		
		// Set metatable
		luaL_getmetatable(L, "gfx.framebuffer");
		lua_setmetatable(L, -2);

		// Set member functions
		lua_setfield(L, -2, "instance");
		lua_pushcfunction(L, gfx_attachframebuffer);
		lua_setfield(L, -2, "attach");
		lua_pushcfunction(L, gfx_detachframebuffer);
		lua_setfield(L, -2, "detach");
		//lua_pushcfunction(L, gfx_bindshader);
		//lua_setfield(L, -2, "bind");
		//lua_pushcfunction(L, gfx_setuniformf);
		//lua_setfield(L, -2, "setuniformf");

		return 1;
	}
	else
	{
		lua_pushstring(L, "Invalid argument passed to newframebuffer function!");
		lua_error(L);
	}
	return 0;
}

int Fabric::gfx_attachframebuffer(lua_State *L)
{
	// fbo:attach(tex, color_attachment_id)
	if (lua_gettop(L) == 3)
	{
		App::GetInstance()->Flush();
		lua_getfield(L, 1, "instance");
		Graphics::FrameBufferObject* fbo = *(Graphics::FrameBufferObject**)lua_touserdata(L, -1);
		
		//lua_getfield(L, 2, "instance");
		TexturedQuadBatch* texqb = (TexturedQuadBatch*)App::GetInstance()->m_QuadBatches[lua_tointeger(L, 2)];
		Graphics::Texture* tex = texqb->m_Texture;
		
		// Attach texture to fbo:
		int attachment_lut[] = {GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT,
			                      GL_COLOR_ATTACHMENT4_EXT, GL_COLOR_ATTACHMENT5_EXT, GL_COLOR_ATTACHMENT6_EXT, GL_COLOR_ATTACHMENT7_EXT,
			                      GL_COLOR_ATTACHMENT8_EXT, GL_COLOR_ATTACHMENT9_EXT, GL_COLOR_ATTACHMENT10_EXT, GL_COLOR_ATTACHMENT11_EXT,
			                      GL_COLOR_ATTACHMENT12_EXT, GL_COLOR_ATTACHMENT13_EXT, GL_COLOR_ATTACHMENT14_EXT, GL_COLOR_ATTACHMENT15_EXT};
		
		fbo->Attach(tex, attachment_lut[lua_tointeger(L, 3)-1], true);
		
		return 0;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to attach function! (of framebuffer)");
		lua_error(L);
	}

	return 0;
}

int Fabric::gfx_detachframebuffer(lua_State *L)
{
	// fbo:detach(color_attachment_id)
	if (lua_gettop(L) == 2)
	{
		lua_getfield(L, 1, "instance");
		Graphics::FrameBufferObject* fbo = *(Graphics::FrameBufferObject**)lua_touserdata(L, -1);
		
		// Attach texture to fbo:
		int attachment_lut[] = {GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT,
			                      GL_COLOR_ATTACHMENT4_EXT, GL_COLOR_ATTACHMENT5_EXT, GL_COLOR_ATTACHMENT6_EXT, GL_COLOR_ATTACHMENT7_EXT,
			                      GL_COLOR_ATTACHMENT8_EXT, GL_COLOR_ATTACHMENT9_EXT, GL_COLOR_ATTACHMENT10_EXT, GL_COLOR_ATTACHMENT11_EXT,
			                      GL_COLOR_ATTACHMENT12_EXT, GL_COLOR_ATTACHMENT13_EXT, GL_COLOR_ATTACHMENT14_EXT, GL_COLOR_ATTACHMENT15_EXT};
		
		fbo->Detach(attachment_lut[lua_tointeger(L, 2)-1]);
		
		return 0;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to detach function! (of framebuffer)");
		lua_error(L);
	}

	return 0;
}

int Fabric::gfx_bindframebuffer(lua_State *L)
{
	// gfx.bindframebuffer(fbo)
	if (lua_gettop(L) == 1)
	{
		App::GetInstance()->Flush();
		App::GetInstance()->m_UsingFBO = true;
		
		lua_getfield(L, 1, "instance");
		Graphics::FrameBufferObject* fbo = *(Graphics::FrameBufferObject**)lua_touserdata(L, -1);
		App::GetInstance()->m_gfx->BindFrameBufferObject(fbo);
		glDisable(GL_DEPTH_TEST);
		
		return 0;
	} else if (lua_gettop(L) == 0)
	{
		App::GetInstance()->Flush();
		App::GetInstance()->m_gfx->UnbindFrameBufferObject();
		App::GetInstance()->m_UsingFBO = false;
		glEnable(GL_DEPTH_TEST);
		
		App::GetInstance()->m_gfx->SetViewport(0, 0, App::GetInstance()->m_win->GetWidth(), App::GetInstance()->m_win->GetHeight());
		Math::Mat4 prjmat = Math::Mat4::Ortho(0, App::GetInstance()->m_win->GetWidth(), App::GetInstance()->m_win->GetHeight(), 0, FABRIC_DEPTH_FAR, FABRIC_DEPTH_NEAR);
		App::GetInstance()->m_gfx->SetProjection(&prjmat);

		return 0;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to bindframebuffer function!");
		lua_error(L);
	}

	return 0;
}

int Fabric::gfx_deleteframebuffer(lua_State *L)
{
	if (lua_gettop(L) == 1)
	{
		App::GetInstance()->m_gfx->DestroyFrameBufferObject(*(Graphics::FrameBufferObject**)lua_touserdata(L, 1));
		return 0;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to delete function! (of framebuffer)");
		lua_error(L);
	}

	return 0;
}

int Fabric::luaopen_appgraphics (lua_State *L) {
  const luaL_reg appgraphicslib[] = {
		{"getrawimage", gfx_getrawimage},
    {"_redrawneeded",   gfx__redrawneeded},
    {"loadidentity",   gfx_loadidentity},
    {"loadtexture",   gfx_loadtexture},
		{"newtexture",   gfx_newtexture},
    {"rawtexture",   gfx_rawtexture},
    {"bindtexture",   gfx_bindtexture},

    {"translate",   gfx_translate},
  	{"scale",   gfx_scale},
  	{"rotate",   gfx_rotate},

    {"getcolor",   gfx_getcolor},
    {"setcolor",   gfx_setcolor},
    {"getalpha",   gfx_getalpha},
    {"setalpha",   gfx_setalpha},

		{"clear",   gfx_clear},
		{"setview",   gfx_setview},
		{"setortho",   gfx_setortho},
		{"blending",   gfx_blending},
		{"alphatest",   gfx_alphatest},
    {"setclearcolor",   gfx_setclearcolor},

		{"bindframebuffer",   gfx_bindframebuffer},
		{"newframebuffer",   gfx_newframebuffer},

		{"bindshader",   gfx_bindshader},
		{"createshader",   gfx_createshader},
    
    {"drawcentered",   gfx_drawcentered},
    {"drawquad",   gfx_drawquad},
    {"drawtopleft",   gfx_drawtopleft},
    {NULL, NULL}
    };
  
  luaL_register(L, LUA_APPGRAPHICSLIBNAME, appgraphicslib);

	////////////////////////
	// Push constants/enums
	
	// alphatest enums
  lua_pushnumber(L, GL_NEVER);
  lua_setfield(L, -2, "NEVER");
	lua_pushnumber(L, GL_LESS);
  lua_setfield(L, -2, "LESS");
	lua_pushnumber(L, GL_EQUAL);
  lua_setfield(L, -2, "EQUAL");
	lua_pushnumber(L, GL_LEQUAL);
  lua_setfield(L, -2, "LEQUAL");
	lua_pushnumber(L, GL_GREATER);
  lua_setfield(L, -2, "GREATER");
	lua_pushnumber(L, GL_NOTEQUAL);
  lua_setfield(L, -2, "NOTEQUAL");
	lua_pushnumber(L, GL_GEQUAL);
  lua_setfield(L, -2, "GEQUAL");
	lua_pushnumber(L, GL_ALWAYS);
  lua_setfield(L, -2, "ALWAYS");

	// blending enums
  lua_pushnumber(L, GL_ZERO);
  lua_setfield(L, -2, "ZERO");
	lua_pushnumber(L, GL_ONE);
  lua_setfield(L, -2, "ONE");
	lua_pushnumber(L, GL_SRC_COLOR);
  lua_setfield(L, -2, "SRC_COLOR");
	lua_pushnumber(L, GL_ONE_MINUS_SRC_COLOR);
  lua_setfield(L, -2, "ONE_MINUS_SRC_COLOR");
	lua_pushnumber(L, GL_DST_COLOR);
  lua_setfield(L, -2, "DST_COLOR");
	lua_pushnumber(L, GL_ONE_MINUS_DST_COLOR);
  lua_setfield(L, -2, "ONE_MINUS_DST_COLOR");
	lua_pushnumber(L, GL_SRC_ALPHA);
  lua_setfield(L, -2, "SRC_ALPHA");
	lua_pushnumber(L, GL_ONE_MINUS_SRC_ALPHA);
  lua_setfield(L, -2, "ONE_MINUS_SRC_ALPHA");
	lua_pushnumber(L, GL_DST_ALPHA);
  lua_setfield(L, -2, "DST_ALPHA");
	lua_pushnumber(L, GL_ONE_MINUS_DST_ALPHA);
  lua_setfield(L, -2, "ONE_MINUS_DST_ALPHA");
	lua_pushnumber(L, GL_CONSTANT_COLOR);
  lua_setfield(L, -2, "CONSTANT_COLOR");
	lua_pushnumber(L, GL_ONE_MINUS_CONSTANT_COLOR);
  lua_setfield(L, -2, "ONE_MINUS_CONSTANT_COLOR");
	lua_pushnumber(L, GL_CONSTANT_ALPHA);
  lua_setfield(L, -2, "CONSTANT_ALPHA");
	lua_pushnumber(L, GL_ONE_MINUS_CONSTANT_ALPHA);
  lua_setfield(L, -2, "ONE_MINUS_CONSTANT_ALPHA");
	lua_pushnumber(L, GL_SRC_ALPHA_SATURATE);
  lua_setfield(L, -2, "SRC_ALPHA_SATURATE");

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
	
	// set __gc for gfx.framebuffer
	luaL_newmetatable(L, "gfx.framebuffer");
	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, gfx_deleteframebuffer);
	lua_settable(L, -3);

  /*lua_pushnumber(L, PI);
  lua_setfield(L, -2, "pi");
  lua_pushnumber(L, HUGE_VAL);
  lua_setfield(L, -2, "huge");*/
	
  return 1;
}
