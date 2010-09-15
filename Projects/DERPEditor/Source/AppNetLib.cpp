#include "AppNetLib.h"

#include <Pxf/Network/NetworkDevice.h>

int DERPEditor::net_createserver(lua_State *L)
{
	return 0;
}

int DERPEditor::luaopen_appnet(lua_State *L)
{
	const luaL_reg appnetlib[] = {
		{"createserver", net_createserver}
	};

	luaL_register(L, LUA_APPNETLIBNAME, appnetlib);

	return 1;
}