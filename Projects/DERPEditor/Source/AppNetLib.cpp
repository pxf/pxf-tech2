#include "AppNetLib.h"

#include <Pxf/Kernel.h>
#include <Pxf/Base/Stream.h>
#include <Pxf/Base/Memory.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Path.h>

#include <Pxf/Base/Hash.h>

#include <Pxf/Network/NetworkDevice.h>
#include <Pxf/Network/Packet.h>

#include <string.h>

using namespace Pxf::Network;
using namespace Pxf;

/* template, lol
int DERPEditor::net_server_UNDEF(lua_State *L)
{
	if (lua_gettop(L) == 0)
	{
		return 0;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to UNDEF function!");
		lua_error(L);
	}

	return 0;
}
*/


int DERPEditor::net_addtag(lua_State *L)
{
	if (lua_gettop(L) == 1)
	{
		char* tag = new char[strlen(lua_tolstring(L, -1, NULL))];
		strcpy(tag, lua_tolstring(L, -1, NULL));
		int type = LuaApp::GetInstance()->m_net->AddTag(tag);
		lua_pushnumber(L, type);

		return 1;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to addtag function!");
		lua_error(L);
	}

	return 0;
}

int DERPEditor::net_gettags(lua_State *L)
{
	if (lua_gettop(L) == 0)
	{
		Util::Array<char*>* tags = LuaApp::GetInstance()->m_net->GetTags();

		lua_newtable(L);

		for(int i=0; i<tags->size(); i++)
		{
			lua_pushnumber(L, i);
			lua_setfield(L, -2, (*tags)[i]);
		}

		return 1;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to gettags function!");
		lua_error(L);
	}

	return 0;
}

int DERPEditor::net_send_texture(lua_State *L)
{
	if (lua_gettop(L) == 1)
	{
		const char* path = lua_tostring(L, 1);
		FileStream s;

		if (!s.OpenReadBinary(path))
			return 0;

		unsigned fsize = s.GetSize();
		char* data = new char[fsize];
		s.Read(data, fsize);
		unsigned long hash = Hash(data, fsize);
		s.Close();

		Packet* packet = LuaApp::GetInstance()->m_net->CreateEmptyPacket("", 1 /* datacache */);

		// hash, filename, datalen, data
		packet->PushInt(hash);
		const char* filename = PathFileName(path);
		packet->PushString(filename, strlen(filename)+1);
		packet->PushInt(fsize);
		packet->PushString(data, fsize);

//		net_packet_push(L, packet);
		Network::NetworkDevice* n = LuaApp::GetInstance()->m_net;
		int num = n->NumClients();

		for (;num>0;num--)
		{
			Network::Client* c = n->GetClient(num-1);
			c->SendPacket(packet);
		}

		delete packet;
		delete data;

		char ret[256];
		Format(ret, "%X_%s\0", hash, filename);

		lua_pushstring(L, ret);
		
		return 1;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to send_texture function!");
		lua_error(L);
	}

	return 0;
}

//
// ------------- CLIENT
//

int DERPEditor::net_createclient(lua_State *L)
{
	if (lua_gettop(L) == 0)
	{
		lua_newtable(L);
		Client** client = (Client**)lua_newuserdata(L, sizeof(Client*));
		luaL_getmetatable(L, "net.client");
		lua_setmetatable(L, -2);

		*client = LuaApp::GetInstance()->m_net->CreateClient();

		lua_setfield(L, -2, "instance");
		lua_pushcfunction(L, net_client_connect);
		lua_setfield(L, -2, "connect");
		lua_pushcfunction(L, net_client_send);
		lua_setfield(L, -2, "send");
		lua_pushcfunction(L, net_client_recv);
		lua_setfield(L, -2, "recv");
		lua_pushcfunction(L, net_client_recv_noblock);
		lua_setfield(L, -2, "recv_noblock");
		lua_pushcfunction(L, net_client_disconnect);
		lua_setfield(L, -2, "disconnect");
		// TODO: Put these two in send instead.
		lua_pushcfunction(L, net_client_send_id);
		lua_setfield(L, -2, "send_id");
		lua_pushcfunction(L, net_client_send_packet);
		lua_setfield(L, -2, "send_packet");

		lua_pushcfunction(L, net_client_connected);
		lua_setfield(L, -2, "connected");

		return 1;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to createclient function!");
		lua_error(L);
	}

	return 0;
}

int DERPEditor::net_client_connect(lua_State *L)
{
	if (lua_gettop(L) == 3)
	{
		lua_getfield(L, -3, "instance");
		Client* client = *(Client**)lua_touserdata(L, -1);
		bool ret = client->Connect(lua_tolstring(L, -3, NULL), lua_tonumber(L, -2));
//		Message("aoeu", "connecting to %s", lua_tolstring(L, -3, NULL));

		if (!ret)
		{
			lua_pushstring(L, "Couldn't connect.");
			return 1;
		}

		return 0;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to connect function!");
		lua_error(L);
	}

	return 0;
}

int DERPEditor::net_client_connected(lua_State *L)
{
	if (lua_gettop(L) == 1)
	{
		lua_getfield(L, -1, "instance");
		Client* client = *(Client**)lua_touserdata(L, -1);

		lua_pushboolean(L, (bool)client->Connected());

		return 1;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to connected function!");
		lua_error(L);
	}

	return 0;
}

int DERPEditor::net_client_disconnect(lua_State *L)
{
	if (lua_gettop(L) == 1)
	{
		lua_getfield(L, -1, "instance");
		Client* client = *(Client**)lua_touserdata(L, -1);
		client->Disconnect();

		return 0;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to disconnect function!");
		lua_error(L);
	}

	return 0;
}

int DERPEditor::net_client_send(lua_State *L)
{
	if (lua_gettop(L) == 3)
	{
		lua_getfield(L, -3, "instance");
		Client* client = *(Client**)lua_touserdata(L, -1);

		int channel = 0;

		if (lua_isstring(L, -3))
		{
			// Fetch the number connected to the string.
			Util::Array<char*>* tags = LuaApp::GetInstance()->m_net->GetTags();

			int i;
			for (i=0; i < tags->size(); i++)
			{
				if (strcmp((*tags)[i], lua_tolstring(L, -3, NULL)) == 0)
					// Found it.
					break;
			}

			if (i == tags->size())
				// Couldn't find it.
				channel = 0;
			else
				channel = i;

			const char* message = lua_tolstring(L, -2, NULL);
			client->Send(channel, message, strlen(message)+1);
		}
		else if (lua_isnumber(L, -3))
		{
			const char* message = lua_tolstring(L, -2, NULL);
			client->Send(lua_tonumber(L, -3), message, strlen(message)+1);
		}

		return 0;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to send function!");
		lua_error(L);
	}

	return 0;
}

int DERPEditor::net_client_send_id(lua_State *L)
{
	if (lua_gettop(L) == 4)
	{
		lua_getfield(L, -4, "instance");
		Client* client = *(Client**)lua_touserdata(L, -1);

		int channel = 0;

		if (lua_isstring(L, -3))
		{
			const char* type = lua_tolstring(L, -3, NULL);

			// Fetch the number connected to the string.
			Util::Array<char*>* tags = LuaApp::GetInstance()->m_net->GetTags();

			int i;
			for (i=0; i < tags->size(); i++)
			{
				if (strcmp((*tags)[i], type) == 0)
					// Found it.
					break;
			}

			if (i == tags->size())
				// Couldn't find it.
				channel = 0;
			else
				channel = i;

			const char* message = lua_tolstring(L, -2, NULL);
			const char* id = lua_tolstring(L, -4, NULL);
			client->SendID(id, channel, message, strlen(message)+1);
		}
		else if (lua_isnumber(L, -3))
		{
			const char* message = lua_tolstring(L, -2, NULL);
			client->SendID(lua_tolstring(L, -4, NULL), lua_tonumber(L, -3)
				, message, strlen(message)+1);
		}

		return 0;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to send_id function!");
		lua_error(L);
	}

	return 0;
}

int DERPEditor::net_client_send_packet(lua_State *L)
{
	if (lua_gettop(L) == 2)
	{
		lua_getfield(L, -2, "instance");
		Client* client = *(Client**)lua_touserdata(L, -1);
		lua_getfield(L, -2, "instance");
		Packet* packet = *(Packet**)lua_touserdata(L, -1);

		client->SendPacket(packet);

		return 0;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to send function!");
		lua_error(L);
	}

	return 0;
}

int DERPEditor::net_client_recv(lua_State *L)
{
	if (lua_gettop(L) == 1)
	{
		lua_getfield(L, -1, "instance");
		Client* client = *(Client**)lua_touserdata(L, -1);
		Packet* packet = client->Recv();

		net_packet_push(L, packet);

		return 1;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to recv function!");
		lua_error(L);
	}

	return 0;
}

int DERPEditor::net_client_recv_noblock(lua_State *L)
{
	if (lua_gettop(L) == 2)
	{
		lua_getfield(L, -2, "instance");
		Client* client = *(Client**)lua_touserdata(L, -1);
		Packet* packet = client->RecvNonBlocking(lua_tonumber(L, -2));

		if (packet != NULL)
			net_packet_push(L, packet);
		else
			return 0;

		return 1;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to recv_noblock function!");
		lua_error(L);
	}

	return 0;
}

int DERPEditor::net_client_delete(lua_State *L)
{
	if (lua_gettop(L) == 1)
	{
		delete (*(Client**)lua_touserdata(L, 1));
		return 0;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to client:delete function!");
		lua_error(L);
	}

	return 0;
}

//
// ------------- SERVER
//

int DERPEditor::net_createserver(lua_State *L)
{
	if (lua_gettop(L) == 0)
	{
		lua_newtable(L);
		Server** server = (Server**)lua_newuserdata(L, sizeof(Server*));
		luaL_getmetatable(L, "net.server");
		lua_setmetatable(L, -2);

		*server = LuaApp::GetInstance()->m_net->CreateServer();

		lua_setfield(L, -2, "instance");
		lua_pushcfunction(L, net_server_bind);
		lua_setfield(L, -2, "bind");
		lua_pushcfunction(L, net_server_send_all);
		lua_setfield(L, -2, "send_all");
		lua_pushcfunction(L, net_server_send);
		lua_setfield(L, -2, "send");
		lua_pushcfunction(L, net_server_recv);
		lua_setfield(L, -2, "recv");
		lua_pushcfunction(L, net_server_recv_noblock);
		lua_setfield(L, -2, "recv_noblock");

		return 1;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to createserver function!");
		lua_error(L);
	}

	return 0;
}

int DERPEditor::net_server_bind(lua_State *L)
{
	if (lua_gettop(L) == 2)
	{
		lua_getfield(L, -2, "instance");
		Server* server = *(Server**)lua_touserdata(L, -1);
		server->Bind(lua_tonumber(L, -2));

		return 0;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to bind function!");
		lua_error(L);
	}

	return 0;
}

int DERPEditor::net_server_send(lua_State *L)
{
	if (lua_gettop(L) == 2)
	{
		lua_getfield(L, -3, "instance");
		Server* server = *(Server**)lua_touserdata(L, -1);
		server->Send(lua_tonumber(L, -2), 0, lua_tolstring(L, -3, NULL));

		return 0;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to send function!");
		lua_error(L);
	}

	return 0;
}

int DERPEditor::net_server_send_all(lua_State *L)
{
	if (lua_gettop(L) == 2)
	{
		lua_getfield(L, -2, "instance");
		Server* server = *(Server**)lua_touserdata(L, -1);
		// TODO: Fix the channel.
		server->SendAll(0, lua_tolstring(L, -2, NULL));

		return 0;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to send_all function!");
		lua_error(L);
	}

	return 0;
}

int DERPEditor::net_server_recv(lua_State *L)
{
	if (lua_gettop(L) == 0)
	{
		lua_getfield(L, -1, "instance");
		Server* server = *(Server**)lua_touserdata(L, -1);
		Packet* packet = server->Recv();

		net_packet_push(L, packet);

		return 1;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to recv function!");
		lua_error(L);
	}

	return 0;
}

int DERPEditor::net_server_recv_noblock(lua_State *L)
{
	if (lua_gettop(L) == 1)
	{
		lua_getfield(L, -2, "instance");
		Server* server = *(Server**)lua_touserdata(L, -1);
		Packet* packet = server->RecvNonBlocking(lua_tonumber(L, -2));

		if (packet != NULL)
			net_packet_push(L, packet);
		else
			return 0;

		return 1;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to recv_noblock function!");
		lua_error(L);
	}

	return 0;
}

int DERPEditor::net_server_delete(lua_State *L)
{
	if (lua_gettop(L) == 1)
	{
		// TODO: Disconnect all users?
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

//
// ------------- PACKET
//

int DERPEditor::net_packet_push(lua_State *L, Packet* _Packet)
{
	lua_newtable(L);
	Packet** packet = (Packet**)lua_newuserdata(L, sizeof(Packet*));
	luaL_getmetatable(L, "net.packet");
	lua_setmetatable(L, -2);

	*packet = _Packet;

	lua_setfield(L, -2, "instance");
	lua_pushnumber(L, _Packet->GetSender());
	lua_setfield(L, -2, "sender");
	if (_Packet->GetData() != NULL)
	{
		lua_pushlstring(L, _Packet->GetData(), _Packet->GetLength());
		lua_setfield(L, -2, "data");
	}
	lua_pushnumber(L, _Packet->GetTag());
	lua_setfield(L, -2, "tag");
	lua_pushstring(L, _Packet->GetID());
	lua_setfield(L, -2, "id");
	lua_pushcfunction(L, net_packet_push_object);
	lua_setfield(L, -2, "push_object");
	lua_pushcfunction(L, net_packet_get_object);
	lua_setfield(L, -2, "get_object");

	return 1;
}

int DERPEditor::net_packet_delete(lua_State *L)
{
	if (lua_gettop(L) == 1)
	{
		delete (*(Packet**)lua_touserdata(L, 1));
		return 0;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to delete function!");
		lua_error(L);
	}

	return 0;
}

// create_packet((int)tag, (char*)id)
int DERPEditor::net_packet_create_empty(lua_State *L)
{
	if (lua_gettop(L) == 2)
	{
		Packet* packet = LuaApp::GetInstance()->m_net->CreateEmptyPacket(
			(char*)lua_tolstring(L, -1, NULL) // Id
			, (int)lua_tonumber(L, -2) // Tag
		);

		net_packet_push(L, packet);

		return 1;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to create_empty function!");
		lua_error(L);
	}

	return 0;
}

// packet:push_object(object)
int DERPEditor::net_packet_push_object(lua_State *L)
{
	if (lua_gettop(L) == 2)
	{
		lua_getfield(L, -2, "instance");
		Packet* packet = *(Packet**)lua_touserdata(L, -1);

		if (lua_isnumber(L, -2))
		{
			const int n = lua_tonumber(L, -2);
			packet->PushInt(n);
		}
		else if (lua_isstring(L, -2))
		{
			const char* str = lua_tolstring(L, -2, NULL);
			packet->PushString(str, strlen(str));
		}

		return 0;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to push_object function!");
		lua_error(L);
	}

	return 0;
}

// packet:get_object((int)pos)
int DERPEditor::net_packet_get_object(lua_State *L)
{
	if (lua_gettop(L) == 2 || lua_gettop(L) == 3)
	{
		bool raw = false;
		if (lua_gettop(L) == 3)
			raw = lua_toboolean(L, 3);

		lua_getfield(L, 1, "instance");
		Packet* packet = *(Packet**)lua_touserdata(L, -1);

		int pos = lua_tonumber(L, 2);
		int type = packet->ObjectType(pos);

		switch (type)
		{
			case 0:
			{
				int n = packet->GetObject<int>(pos);
				lua_pushnumber(L, n);
				return 1;
				break;
			}
			case 2:
			{
				int strl = packet->ObjectSize(pos);
				char* str = packet->GetArray<char*>(strl, pos);
				if (raw)
					return net_raw_data_push(L, str, strl);

				lua_pushstring(L, str);
				return 1;
				break;
			}
		}

		return 0;
	}
	else
	{
		lua_pushstring(L, "Invalid arguments passed to get_object function!");
		lua_error(L);
	}

	return 0;
}

//
// ------------- RAW_DATA
//

int DERPEditor::net_raw_data_push(lua_State *L, void* _Data, int _Size)
{
	lua_newtable(L);

//	Packet** packet = (Packet**)lua_newuserdata(L, sizeof(Packet*));
	void** data = (void**)lua_newuserdata(L, _Size);
	luaL_getmetatable(L, "net.raw_data");
	lua_setmetatable(L, -2);

//	*packet = _Packet;
	*data = _Data;
	//*data = new char[_Size];
	//MemoryCopy(*data, _Data, _Size);

	lua_setfield(L, -2, "instance");

	return 1;
}

int DERPEditor::net_raw_data_delete(lua_State *L)
{
	if (lua_gettop(L) == 1)
	{
		delete (*(char**)lua_touserdata(L, 1));
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
		{"addtag", net_addtag},
		{"gettags", net_gettags},
		{"create_packet", net_packet_create_empty},
		{"send_texture", net_send_texture},
		{NULL, NULL}
	};

	luaL_register(L, LUA_APPNETLIBNAME, appnetlib);
 
	// set __gc for net.server
	luaL_newmetatable(L, "net.server");
	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, net_server_delete);
	lua_settable(L, -3); 

	// set __gc for net.client
	luaL_newmetatable(L, "net.client");
	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, net_client_delete);
	lua_settable(L, -3); 

	// set __gc for net.packet
	luaL_newmetatable(L, "net.packet");
	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, net_packet_delete);
	lua_settable(L, -3); 

	// set __gc for net.packet
	luaL_newmetatable(L, "net.raw_data");
	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, net_raw_data_delete);
	lua_settable(L, -3); 

	return 1;
}
