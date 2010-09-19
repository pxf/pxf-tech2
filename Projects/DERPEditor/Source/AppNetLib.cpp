#include "AppNetLib.h"

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

#include <Pxf/Network/NetworkDevice.h>

using namespace Pxf::Network;
using namespace Pxf;

int DERPEditor::net_createserver(lua_State *L)
{
	if (lua_gettop(L) == 0)
	{

		lua_newtable(L);
    Server** server = (Server**)lua_newuserdata(L, sizeof(Server*));
		luaL_getmetatable(L, "net.server");
    lua_setmetatable(L, -2);
    
    *server = LuaApp::GetInstance()->m_net->CreateServer();
    
		/*lua_newtable(L);
		lua_pushcfunction(L, net_server_delete);
		lua_setfield(L, -2, "__gc");
		lua_setmetatable(L, -2);*/
		lua_setfield(L, -2, "instance");
		lua_pushcfunction(L, net_server_bind);
		lua_setfield(L, -2, "bind");

		return 1;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to createserver function!");
		lua_error(L);
	}

	return 0;
}

int DERPEditor::net_createclient(lua_State *L)
{
	if (lua_gettop(L) == 0)
	{
		Client* client = LuaApp::GetInstance()->m_net->CreateClient();
		lua_pushnumber(L, client->Ident);
		return 1;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to createclient function!");
		lua_error(L);
	}

	return 0;
}

int DERPEditor::net_server_bind(lua_State *L)
{
	if (lua_gettop(L) == 2)
	{
		lua_getfield(L, -2, "instance");
		Server* server = (Server*)lua_touserdata(L, -1);
		Message("aoeu", "IDENT %d", server->Ident);
		server->Bind(lua_tonumber(L, -2));
		return 1;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to bind function!");
		lua_error(L);
	}

	return 0;
}

int DERPEditor::net_server_delete(lua_State *L)
{
	Message("net", "Deleting server");

	if (lua_gettop(L) == 1)
	{
    delete (*(Server**)lua_touserdata(L, 1));
		return 0;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to delete function!");
		lua_error(L);
	}

	return 0;
}

int DERPEditor::luaopen_appnet(lua_State *L)
{
  
	const luaL_reg appnetlib[] = {
		{"createserver", net_createserver},
		{"createclient", net_createclient},
		{NULL, NULL}
	};

	luaL_register(L, LUA_APPNETLIBNAME, appnetlib);
 
  // set __gc for net.server
	luaL_newmetatable(L, "net.server");
  lua_pushstring(L, "__gc");
  lua_pushcfunction(L, net_server_delete);
  lua_settable(L, -3); 

	return 1;
}