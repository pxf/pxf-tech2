#ifndef __DERPEDITOR_LUAAPP_H__
#define __DERPEDITOR_LUAAPP_H__

// Lua includes
extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}


#define LUAAPP_TABLE "app"


namespace DERPEditor
{
    class LuaApp
    {
    public:
        LuaApp(const char* _filepath);
        ~LuaApp();
        
        void CleanUp();
        bool Load();
        bool Reload();

    private:
        const char* m_Filepath;
        
        // Lua states
        lua_State* L;
        bool m_Running;
        bool m_Started;
        
        // Register lua libs and callbacks
        void _register_lua_libs_callbacks();
        void _register_own_callbacks();
        
        // Callback methods
        static int Print(lua_State *_L);
    };
    
} /* DERPEditor */

#endif /* __DERPEDITOR_LUAAPP_H__ */