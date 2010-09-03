#ifndef __DERPEDITOR_LUAAPP_H__
#define __DERPEDITOR_LUAAPP_H__

#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Input/InputDevice.h>

#include "TexturedQuadBatch.h"

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
        bool Boot();
        bool Reboot();
        
        bool Update();
        void Draw();

    private:
        const char* m_Filepath;
        
        // Pointers to engine systems
        Pxf::Graphics::GraphicsDevice* m_gfx;
        Pxf::Input::InputDevice* m_inp;
        
        // Graceful application fail
        TexturedQuadBatch* m_AppErrorQB;
        
        // Lua states
        lua_State* L;
        bool m_Running;
        bool m_Started;
        bool m_Shutdown;
        
        // Register lua libs and callbacks
        void _register_lua_libs_callbacks();
        void _register_own_callbacks();
        
        // Callback methods
        static int Print(lua_State *_L);
    };
    
} /* DERPEditor */

#endif /* __DERPEDITOR_LUAAPP_H__ */