#ifndef __FABRIC_APP_H__
#define __FABRIC_APP_H__

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Timer.h>
#include <Pxf/Util/Map.h>

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


#define FABRIC_TABLE "app"

#define FABRIC_MAXQB 8
//#define FABRIC_QBSIZE 2048
#define FABRIC_DEPTH_RANGE 200.0f
#define FABRIC_DEPTH_FAR 100.0f
#define FABRIC_DEPTH_NEAR -100.0f
//#define FABRIC_DEPTH_STEP (FABRIC_DEPTH_RANGE / (FABRIC_QBSIZE * FABRIC_MAXQB))

#define FABRIC_REDRAWMODE_FULL 0
#define FABRIC_REDRAWMODE_NORMAL 1

// TODO: Fix setting for doublepass
#define FABRIC_DOUBLEPASS_OFF 2
#define FABRIC_DOUBLEPASS_ON 1


namespace Fabric
{
    class App
    {
    public:
        App(Pxf::Graphics::Window* _win, const char* _filepath);
        ~App();
        
        void CleanUp();
        void Init();
        bool Boot();
        void Reboot();
        void Shutdown();
        
        bool Update();
        void Draw();
        void Flush();
        bool GuiHit(); // Returns true if the gui handled input last update
        
        // "snigelton" honk honk
        static App* GetInstance();

				// Drawstate
				bool m_Dirty;
				void SetDirty() { m_Dirty = true; };
				bool m_UsingFBO;
				
				// Frame stats
				int m_FlushCount;
				int m_SuccessFlushCount;
				int m_QuadCount;
        
        // QuadBatches
        unsigned int m_QuadBatchCount;
        QuadBatch* m_QuadBatches[FABRIC_MAXQB];

				// Special quad batches
				Pxf::Util::Array<TexturedQuadBatch*> m_RawTextureQB;
        
        // Depth control
        int m_MaxQuadCount;
        float m_DepthStep;
        
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

				// Bind external functions
				void BindExternalFunction(const char* _funcname, lua_CFunction _funcp);
				Pxf::Util::Map<const char*, lua_CFunction> m_ExternalFuncs;
        
        // Pointers to engine systems
        Pxf::Graphics::GraphicsDevice* m_gfx;
        Pxf::Input::InputDevice* m_inp;
        Pxf::Graphics::Window* m_win;
				Pxf::Audio::AudioDevice* m_snd;
				Pxf::Network::NetworkDevice* m_net;
        
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
        bool m_Reboot;
        int m_QuadBatchCurrent;
        
        // Timers
        Pxf::Timer m_TimerUpdate;
        Pxf::Timer m_TimerDraw;
        
        // Register lua libs and callbacks
        void _register_lua_libs_callbacks();
        void _register_own_callbacks();
        
        // Callback methods
        static int Print(lua_State *_L);
		static int LoadFile(lua_State *_L);
		static int ImportFile(lua_State *_L);
    };
    
} /* Fabric */

#endif /* __FABRIC_APP_H__ */