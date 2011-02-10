#include "AppSoundLib.h"

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Audio/AudioDevice.h>

int Fabric::snd_stopsound(lua_State *L)
{
	// snd.stopsound(id)
	if(lua_gettop(L) == 1)
	{
		App::GetInstance()->m_snd->Stop(lua_tointeger(L,1));
		return 0;
	}
	else
	{
		lua_pushstring(L, "Invalid argument passed to stopsound function!");
		lua_error(L);
	}

	return 0;
}

int Fabric::snd_stopall(lua_State *L)
{
	// snd.pauseall()
	if(lua_gettop(L) == 0)
	{
		App::GetInstance()->m_snd->StopAll();
		return 0;
	}
	else
	{
		lua_pushstring(L, "Invalid argument passed to stopsound function!");
		lua_error(L);
	}

	return 0;
}

int Fabric::snd_pauseall(lua_State *L)
{
	// snd.pauseall(id)
	if(lua_gettop(L) == 0)
	{
		App::GetInstance()->m_snd->PauseAll();
		return 0;
	}
	else
	{
		lua_pushstring(L, "Invalid argument passed to stopsound function!");
		lua_error(L);
	}

	return 0;
}

int Fabric::snd_pausesound(lua_State* L)
{
	// snd.pausesound(id)
	if(lua_gettop(L) == 1)
	{
		App::GetInstance()->m_snd->Pause(lua_tointeger(L,1));
		return 0;
	}
	else
	{
		lua_pushstring(L, "Invalid argument passed to stopsound function!");
		lua_error(L);
	}

	return 0;
}

int Fabric::snd_playsound(lua_State *L) 
{
	// snd.playsound(id)
	if(lua_gettop(L) == 1)
	{
		App::GetInstance()->m_snd->Play(lua_tointeger(L, 1), false);
		return 0;
	}
	// snd.playsound(id,loop)
	else if(lua_gettop(L) == 2)
	{
		App::GetInstance()->m_snd->Play(lua_tointeger(L,1),lua_toboolean(L,2));
	}
	else
	{
		lua_pushstring(L, "Invalid argument passed to playsound function!");
		lua_error(L);
	}

	return 0;
}

int Fabric::snd_newsound(lua_State *L) 
{
	// snd.newsound(path)
	if(lua_gettop(L) == 1)
	{
		int _NewID = App::GetInstance()->m_snd->RegisterSound(lua_tostring(L, 1));
		lua_pushnumber(L, _NewID); // push qb index as result
		return 1;
	}
	else
	{
		lua_pushstring(L, "Invalid argument passed to loadtexture function!");
		lua_error(L);
	}

	return 0;
}

int Fabric::luaopen_appsound(lua_State *L)
{
	const luaL_reg appsoundlib[] = {
		{"playsound",   snd_playsound},
		{"stopsound",   snd_stopsound},
		{"stopall",   snd_stopall},
		{"pauseall",   snd_pauseall},
		{"newsound",   snd_newsound},
		{NULL, NULL}
    };
  
	luaL_register(L, FABRIC_SOUNDLIBNAME, appsoundlib);

	return 1;
}