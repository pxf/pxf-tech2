#ifndef __DERPEDITOR_LUAAPP_H__
#define __DERPEDITOR_LUAAPP_H__

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Timer.h>

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

#define LUAAPP_MAXQB 8
#define LUAAPP_QBSIZE 2048
#define LUAAPP_DEPTH_RANGE 200.0f
#define LUAAPP_DEPTH_FAR 100.0f
#define LUAAPP_DEPTH_NEAR -100.0f
#define LUAAPP_DEPTH_STEP (LUAAPP_DEPTH_RANGE / (LUAAPP_QBSIZE * LUAAPP_MAXQB))

#define LUAAPP_REDRAWMODE_FULL 0
#define LUAAPP_REDRAWMODE_NORMAL 1
#define LUAAPP_REDRAWMODE_STENCIL 2


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
        void Shutdown();
        
        bool Update();
        void Draw();
        
        // "snigelton" honk honk
        static LuaApp* GetInstance();
        
        // QuadBatches
        unsigned int m_QuadBatchCount;
        TexturedQuadBatch* m_QuadBatches[LUAAPP_MAXQB];
        
        // QuadBatch control
        int ChangeActiveQB(unsigned int _id);
        QuadBatch* GetActiveQB();
        void IncDepth();
        void ResetDepth();
        
        // Redraw control
        void Redraw(); // redraws full screen (no stencil test)
        void Redraw(int x, int y, int w, int h); // adds a redraw region (adds stencil test)
        bool m_RedrawNeeded;
        bool m_RedrawFull;
        bool m_RedrawStencil;
        unsigned int m_RedrawMode;
        QuadBatch* m_StencilQB;
        
        // Matrises
        Pxf::Math::Mat4 m_TransformMatrix;
        
        // Public Lua states
        float m_CurrentDepth;
        Pxf::Math::Vec4f m_CurrentColor;
        
        // Error handling
        bool HandleErrors(int _error);
        bool CallScriptFunc(const char* _funcname, int nargs = 0);
        
        // Pointers to engine systems
        Pxf::Graphics::GraphicsDevice* m_gfx;
        Pxf::Input::InputDevice* m_inp;
        Pxf::Graphics::Window* m_win;
		Pxf::Audio::AudioDevice* m_snd;
        
    private:
        const char* m_Filepath;
        
        // Graceful application fail
        TexturedQuadBatch* m_AppErrorQB;
        
        // Lua states
        lua_State* L;
        // TODO: Add redraw region.
        bool m_Running;
        bool m_Started;
        bool m_Shutdown;
        int m_QuadBatchCurrent;
        
        // Timers
        Pxf::Timer m_TimerUpdate;
        Pxf::Timer m_TimerDraw;
        
        // Register lua libs and callbacks
        void _register_lua_libs_callbacks();
        void _register_own_callbacks();
        
        // Callback methods
        static int Print(lua_State *_L);
    };
    
} /* DERPEditor */

#endif /* __DERPEDITOR_LUAAPP_H__ */