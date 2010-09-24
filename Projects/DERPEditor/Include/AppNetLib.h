#ifndef __DERPEDITOR_APPNETLIB_H__
#define __DERPEDITOR_APPNETLIB_H__

#include "LuaApp.h"

#define LUA_LIB
#include "lua.h"
#include "lauxlib.h"

#define LUA_APPNETLIBNAME "net"

#include <Pxf/Network/NetworkDevice.h>

namespace DERPEditor
{
	// lib functions
	int net_createserver(lua_State *L);
	int net_createclient(lua_State *L);
	int net_addtag(lua_State *L);
	int net_gettags(lua_State *L);

	// packet
	int net_packet_push(lua_State *L, Pxf::Network::Packet* _Packet);
	int net_packet_delete(lua_State *L);

	// server functions
	int net_server_bind(lua_State *L);
	int net_server_send(lua_State *);
	int net_server_send_all(lua_State *);
	int net_server_recv(lua_State *);
	int net_server_recv_noblock(lua_State *);
	int net_server_delete(lua_State *L);

	// client functions
	int net_client_connect(lua_State *L);
	int net_client_disconnect(lua_State *L);
	int net_client_send(lua_State *L);
	int net_client_recv(lua_State *L);
	int net_client_recv_noblock(lua_State *L);
	int net_client_delete(lua_State *L);

	// Register lib
	int luaopen_appnet(lua_State *L);
} /* DERPEditor */

#endif /* __DERPEDITOR_APPNETLIB_H__ */