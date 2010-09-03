#ifndef __DERPEDITOR_LUAAPP_H__
#define __DERPEDITOR_LUAAPP_H__

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

#include <Pxf/Graphics/Window.h>
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
        LuaApp(Pxf::Graphics::Window* _win, const char* _filepath);
        ~LuaApp();
        
        void CleanUp();
        void Init();
        bool Boot();
        bool Reboot();
        
        bool Update();
        void Draw();
        
        // "snigelton" honk honk
        static LuaApp* GetInstance();
        
        // QuadBatches
        unsigned int m_QuadBatchCount;
        TexturedQuadBatch* m_QuadBatches[8];
        
        // QuadBatch control
        void ChangeActiveQB(unsigned int _id);
        QuadBatch* GetActiveQB();
        
        // Matrises
        Pxf::Math::Mat4 m_TransformMatrix;
        
        // Public Lua states
        bool m_RedrawNeeded;
        
        // Error handling
        bool HandleErrors(int _error);
        bool CallScriptFunc(const char* _funcname, int nargs = 0);
        
    private:
        const char* m_Filepath;
        
        // Pointers to engine systems
        Pxf::Graphics::GraphicsDevice* m_gfx;
        Pxf::Input::InputDevice* m_inp;
        Pxf::Graphics::Window* m_win;
        
        // Graceful application fail
        TexturedQuadBatch* m_AppErrorQB;
        
        // Lua states
        lua_State* L;
        // TODO: Add redraw region.
        bool m_Running;
        bool m_Started;
        bool m_Shutdown;
        int m_QuadBatchCurrent;
        
        // Register lua libs and callbacks
        void _register_lua_libs_callbacks();
        void _register_own_callbacks();
        
        // Callback methods
        static int Print(lua_State *_L);
    };
    
} /* DERPEditor */

#endif /* __DERPEDITOR_LUAAPP_H__ */