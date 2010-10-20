#include "AppCoreLib.h"
#include <Pxf/Base/Hash.h>
#include <Pxf/Base/Path.h>
#include <sstat.h>

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

int DERPEditor::app_getmemusage(lua_State *L)
{
  // app.getmemusage() -- returns in bytes how much memory the application is using
  if (lua_gettop(L) == 0)
  {
	lua_pushnumber(L, sstat_memoryusage());
    return 1;
    
  } else {
    lua_pushstring(L, "Invalid argument passed to getmemusage function!");
    lua_error(L);
  }
  return 0;
}

int DERPEditor::app_savedialog(lua_State *L)
{
  // app.savedialog(default_filename) -- returns a filepath if the dialog succeeded
  if (lua_gettop(L) == 1)
  {
		char* filepath = new char[1024];
		int ret_size = 0;
		sprintf(filepath, "%s", lua_tostring(L, 1));
		
		if (sstat_savefiledialog(filepath) == 0)
		{
			lua_pushstring(L, filepath);
			ret_size = 1;
		}
		
		delete [] filepath;
    return ret_size;
    
  } else {
    lua_pushstring(L, "Invalid argument passed to savedialog function!");
    lua_error(L);
  }
  return 0;
}

int DERPEditor::app_opendialog(lua_State *L)
{
  // app.opendialog() -- returns a filepath if the dialog succeeded
  if (lua_gettop(L) == 0)
  {
		char* filepath = new char[1024];
		int ret_size = 0;
		
		if (sstat_openfiledialog(filepath) == 0)
		{
			lua_pushstring(L, filepath);
			ret_size = 1;
		}
		
		delete [] filepath;
    return ret_size;
    
  } else {
    lua_pushstring(L, "Invalid argument passed to savedialog function!");
    lua_error(L);
  }
  return 0;
}

int DERPEditor::app__setrenderoption(lua_State *L)
{
  // app.setrenderoption(render_mode) -- changes render option
  if (lua_gettop(L) == 1)
  {
    LuaApp::GetInstance()->m_RedrawMode = lua_tointeger(L, 1);
    return 0;
    
  } else {
    lua_pushstring(L, "Invalid argument passed to setrenderoption function!");
    lua_error(L);
  }
  return 0;
}

int DERPEditor::app__getrenderoption(lua_State *L)
{
  // render_mode = app.getrenderoption() -- get render option
  if (lua_gettop(L) == 0)
  {
    lua_pushnumber(L, LuaApp::GetInstance()->m_RedrawMode);
    return 1;
    
  } else {
    lua_pushstring(L, "Invalid argument passed to getrenderoption function!");
    lua_error(L);
  }
  return 0;
}

int DERPEditor::util_hashfile(lua_State *L)
{
	unsigned long ret = 0;
	if (lua_gettop(L) == 1)
	{
		const char* filename = lua_tostring(L, 1);
		ret = Pxf::HashFile(filename);
	}
	lua_pushinteger(L, ret);
	return 1;
}

int DERPEditor::util_pathext(lua_State *L)
{
	if (lua_gettop(L) == 1)
	{
		const char* filename = lua_tostring(L, 1);
		const char* ext = Pxf::PathExt(filename);
		lua_pushstring(L, ext);
		return 1;
	}
	return 0;
}

int DERPEditor::util_pathfilename(lua_State *L)
{
	if (lua_gettop(L) == 1)
	{
		const char* filename = lua_tostring(L, 1);
		const char* name = Pxf::PathFileName(filename);
		lua_pushstring(L, name);
		return 1;
	}
	return 0;
}

int DERPEditor::luaopen_appcore (lua_State *L) {
  const luaL_reg appcorelib[] = {
    {"reboot",   app_reboot},
    {"quit",   app_quit},
    //{"traceback",   app_traceback},
    {"getwindimensions",   app_getwindimensions},
    
    {"getmemusage",   app_getmemusage},
    {"savedialog",   app_savedialog},
    {"opendialog",   app_opendialog},
	
	{"hashfile", util_hashfile},
    
	{"_setrenderoption",   app__setrenderoption},
    {"_getrenderoption",   app__getrenderoption},
    {NULL, NULL}
    };
  
  luaL_register(L, LUA_APPCORENAME, appcorelib);
  lua_pushinteger(L, LUAAPP_REDRAWMODE_FULL);
  lua_setfield(L, -2, "REDRAWMODE_FULL");
  lua_pushinteger(L, LUAAPP_REDRAWMODE_NORMAL);
  lua_setfield(L, -2, "REDRAWMODE_NORMAL");
  /*lua_pushnumber(L, PI);
  lua_setfield(L, -2, "pi");
  lua_pushnumber(L, HUGE_VAL);
  lua_setfield(L, -2, "huge");*/
  return 1;
}

