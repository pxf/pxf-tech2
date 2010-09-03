#include "LuaApp.h"

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

#include <Pxf/Modules/pri/OpenGL.h>

#include "AppGraphicsLib.h"

#define LOCAL_MSG "LuaApp"

using namespace DERPEditor;
using namespace Pxf;
using namespace Pxf::Graphics;

static LuaApp* DERPEditor::_appinstance;
LuaApp* LuaApp::GetInstance()
{
    return _appinstance;
}

LuaApp::LuaApp(const char* _filepath)
{
    m_Filepath = _filepath;
    
    m_AppErrorQB = new TexturedQuadBatch(4, NULL, "data/apperror.png");
    m_AppErrorQB->Begin();
    m_AppErrorQB->AddCentered(0, 0, 512, 256);
    m_AppErrorQB->End();
    
    m_Started = false;
    m_Running = false;
    m_Shutdown = false;
    
    // get engine system pointers for easy access later on
    m_gfx = Kernel::GetInstance()->GetGraphicsDevice();
    m_inp = Kernel::GetInstance()->GetInputDevice();
    
    // Set "snigelton"
    _appinstance = this;
}

LuaApp::~LuaApp()
{
    CleanUp();
    
    delete m_AppErrorQB;
}

void LuaApp::CleanUp()
{
    m_Started = false;
    m_Running = false;
}

bool LuaApp::Boot()
{
    // Init lua state
    L = lua_open();
    
    // Register lua libs
    _register_lua_libs_callbacks();
    
    // Register own callbacks
    _register_own_callbacks();
    
    ////////////////////////////////
    // Load bootstrap script
    int s = luaL_loadfile(L, "data/bootstrap.lua");
    
    if (!s)
	{
		s = lua_pcall(L, 0, LUA_MULTRET, 0);
		if ( s ) {
			Message(LOCAL_MSG, " [Error while running bootstrap.lua] -- %s", lua_tostring(L, -1));
			lua_pop(L, 1); // remove error message
		} else {
		    
		    // Set app.instance to this class instance!
		    // Instance will later be used to call correct app instance.
            lua_getglobal(L, LUAAPP_TABLE);
            lua_pushlightuserdata(L, this);
            lua_setfield(L, -2, "instance");
            lua_pop(L, 1);
		    
		    //////////////////////////
			// Load main app script   
            s = luaL_loadfile(L, m_Filepath);
        	if (!s)
        	{
        		s = lua_pcall(L, 0, LUA_MULTRET, 0);

        		if ( s ) {
        			Message(LOCAL_MSG, "[Error while running %s] -- %s", m_Filepath, lua_tostring(L, -1));
        			lua_pop(L, 1);
        		} else {
                    m_Running = true;
        		}

        	} else {
        		Message(LOCAL_MSG, "[Error while loading %s] -- %s", m_Filepath, lua_tostring(L,-1));
        	}
		}

	} else {
		Message(LOCAL_MSG, "[Error while loading bootstrap.lua] -- %s",lua_tostring(L,-1));
	}
    
    return false;
}

bool LuaApp::Reboot()
{
    CleanUp();
    return Boot();
}


bool LuaApp::Update()
{
    if (m_Running)
    {
        // TODO: Call application update method
    } else {
        // A application error has occurred, see if the user wants to reboot or quit
        int mx,my;
        m_inp->GetMousePos(&mx, &my);
        
        if (m_inp->IsButtonDown(Input::MOUSE_LEFT))
        {
            if (my > 300 + 34 &&
                my < 300 + 34 + 26)
            {
                // reboot button
                if (mx > 400 - 105 &&
                    mx < 400 + 22)
                    Reboot();
                
                // quit button
                if (mx > 400 + 52 &&
                    mx < 400 + 108)
                    m_Shutdown = true;
                
            }
        }
        
    }
    
    return !m_Shutdown;
}

void LuaApp::Draw()
{
    // Setup viewport and matrises
    // TODO: get window dimensions dynamically
    m_gfx->SetViewport(0, 0, 800, 600);
    
    if (m_Running)
    {
        Math::Mat4 prjmat = Math::Mat4::Ortho(0, 800, 600, 0, -1000.0f, 1000.0f);
        m_gfx->SetProjection(&prjmat);
        glClearColor(46.0f/255.0f,46.0f/255.0f,46.0f/255.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // TODO: Call application draw method etc
    } else {
        Math::Mat4 prjmat = Math::Mat4::Ortho(-400, 400, 300, -300, -1000.0f, 1000.0f);
        m_gfx->SetProjection(&prjmat);
        
        glClearColor(46.0f/255.0f,46.0f/255.0f,46.0f/255.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Display application error
        m_AppErrorQB->Draw();
    }
}



///////////////////////////////////////////////////////////////////
// Private methods
void LuaApp::_register_lua_libs_callbacks()
{
	// Lua libs
	static const luaL_Reg lualibs[] = {
		{"", luaopen_base},
		{LUA_LOADLIBNAME, luaopen_package},
		{LUA_TABLIBNAME, luaopen_table},
		{LUA_IOLIBNAME, luaopen_io},
		{LUA_OSLIBNAME, luaopen_os},
		{LUA_STRLIBNAME, luaopen_string},
		{LUA_MATHLIBNAME, luaopen_math},
		{LUA_DBLIBNAME, luaopen_debug},
		{NULL, NULL}
	};

	for (const luaL_Reg *lib = lualibs; lib->func; lib++) {
		lua_pushcfunction(L, lib->func);
		lua_pushstring(L, lib->name);
		lua_call(L, 1, 0);
	}
}

void LuaApp::_register_own_callbacks()
{
    // Register own callbacks
	lua_register(L, "print", Print);
    
    // Create empty luagame table
    lua_newtable(L);
    lua_setglobal (L, LUAAPP_TABLE);
        
    // Register subsystems
    luaopen_appgraphics(L);
	/*Vec2::RegisterClass(L);
    GraphicsSubsystem::RegisterClass(L);
    ResourcesSubsystem::RegisterClass(L);
	PhysicsSubsystem::RegisterClass(L);*/

    // Sockets
    //luaopen_socket_core(L);
}


///////////////////////////////////////////////////////////////////
// Callback methods

int LuaApp::Print(lua_State *_L)
{
    
    // More or less copy paste from lbaselib.c of Lua dist.
    // Modified so that prints can be pushed to the "game console"
    int n = lua_gettop(_L);  /* number of arguments */
    int i;
    lua_getglobal(_L, "tostring");
    for (i=1; i<=n; i++) {
        const char *s;
        lua_pushvalue(_L, -1);  /* function to be called */
        lua_pushvalue(_L, i);   /* value to print */
        lua_call(_L, 1, 1);
        s = lua_tostring(_L, -1);  /* get result */
        if (s == NULL)
            return luaL_error(_L, LUA_QL("tostring") " must return a string to "
                                  LUA_QL("print"));
        if (i>1)
            fputs("\t", stdout);
        fputs("# ", stdout);
        fputs(s, stdout);
        lua_pop(_L, 1);  /* pop result */
    }
    fputs("\n", stdout);
    return 0;
}


