#include "LuaApp.h"

#include <Pxf/Modules/pri/OpenGL.h>

#include "AppCoreLib.h"
#include "AppInputLib.h"
#include "AppGraphicsLib.h"
#include "AppSoundLib.h"

#define LOCAL_MSG "LuaApp"

using namespace DERPEditor;
using namespace Pxf;
using namespace Pxf::Graphics;

static LuaApp* _appinstance;
LuaApp* LuaApp::GetInstance()
{
    return _appinstance;
}

LuaApp::LuaApp(Graphics::Window* _win, const char* _filepath)
	: m_snd(0)
{
    m_Filepath = _filepath;
    m_win = _win;
    
    m_CurrentDepth = 0.0f;
    m_CurrentColor = Math::Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
    
    m_AppErrorQB = new TexturedQuadBatch(4, "data/apperror.png", &m_CurrentDepth, &m_CurrentColor, NULL);
    m_AppErrorQB->Begin();
    m_AppErrorQB->AddCentered(0, 0, 512, 256);
    m_AppErrorQB->End();
    
    m_StencilQB = new QuadBatch(32, &m_CurrentDepth, &m_CurrentColor);
    
    m_RedrawNeeded = false;
    m_RedrawStencil = false;
    m_RedrawFull = false;
    m_RedrawMode = LUAAPP_REDRAWMODE_FULL;
    
    m_Started = false;
    m_Running = false;
    m_Shutdown = false;
    m_QuadBatchCurrent = -1;
    m_QuadBatchCount = 0;
    
    m_TransformMatrix = Math::Mat4::Identity;
    
    // get engine system pointers for easy access later on
    m_gfx = Kernel::GetInstance()->GetGraphicsDevice();
    m_inp = Kernel::GetInstance()->GetInputDevice();
	m_snd = Kernel::GetInstance()->GetAudioDevice();
    
    // Set "snigelton"
    _appinstance = this;
}

LuaApp::~LuaApp()
{
    CleanUp();
    
    delete m_AppErrorQB;
}

void LuaApp::Init()
{
  m_RedrawMode = LUAAPP_REDRAWMODE_FULL;
  
  // Init GL settings
  Math::Mat4 prjmat = Math::Mat4::Ortho(0, 800, 600, 0, LUAAPP_DEPTH_FAR, LUAAPP_DEPTH_NEAR);
  m_gfx->SetProjection(&prjmat);
  
  //glClearColor(46.0f/255.0f,46.0f/255.0f,46.0f/255.0f,1.0f);
  glClearColor(26.0f/255.0f,26.0f/255.0f,26.0f/255.0f,1.0f);
  
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER,0.1f);
  
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LEQUAL);

  glClearStencil(0x0);
  
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
}

void LuaApp::CleanUp()
{
    // reset states
    m_Started = false;
    m_Running = false;
    
    if (m_QuadBatchCount > 0)
    {
      for(int i = 0; i < m_QuadBatchCount; ++i)
      {
        delete m_QuadBatches[i];
      }
    }
    m_QuadBatchCount = 0;
    m_QuadBatchCurrent = -1;
    m_RedrawStencil = false;
    m_RedrawFull = false;
    
    // reset transform matrix
    m_TransformMatrix = Math::Mat4::Identity;
}

bool LuaApp::Boot()
{
  // Init ourselves
  Init();
  
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
      /*lua_getglobal(L, LUAAPP_TABLE);
      lua_pushlightuserdata(L, this);
      lua_setfield(L, -2, "instance");
      lua_pop(L, 1);*/
		    
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
    		lua_pop(L, 1);
    	}
		}

	} else {
		Message(LOCAL_MSG, "[Error while loading bootstrap.lua] -- %s", lua_tostring(L,-1));
		lua_pop(L, 1);
	}
    
  return false;
}

bool LuaApp::Reboot()
{
  CleanUp();
  return Boot();
}

void LuaApp::Shutdown()
{
  m_Shutdown = true;
}


bool LuaApp::Update()
{
    if (m_Running)
    {
      CallScriptFunc("_update");
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

int LuaApp::ChangeActiveQB(unsigned int _id)
{
  int old = 0;
  if (m_QuadBatchCurrent >= 0)
  {
    old = m_QuadBatchCurrent;
    m_QuadBatches[m_QuadBatchCurrent]->End();
  }
  
  m_QuadBatchCurrent = _id;
  m_QuadBatches[m_QuadBatchCurrent]->Begin();
  return old;
}

QuadBatch* LuaApp::GetActiveQB()
{
  if (m_QuadBatchCurrent < 0)
    return NULL;
  
  return m_QuadBatches[m_QuadBatchCurrent];
}

void LuaApp::IncDepth()
{
  m_CurrentDepth = m_CurrentDepth - LUAAPP_DEPTH_STEP;
}

void LuaApp::ResetDepth()
{
  m_CurrentDepth = LUAAPP_DEPTH_FAR;
}

void LuaApp::Redraw()
{
  m_RedrawNeeded = true;
  m_RedrawFull = true;
  m_RedrawStencil = false;
}

void LuaApp::Redraw(int x, int y, int w, int h)
{
  m_RedrawNeeded = true;
  
  if (m_RedrawMode == LUAAPP_REDRAWMODE_FULL)
    m_RedrawMode = true;
  
  if (!m_RedrawFull)
  {
    m_RedrawStencil = true;
  
    m_StencilQB->Begin();
    m_StencilQB->AddTopLeft(x,y,w,h);
    m_StencilQB->End();
  }
}

void LuaApp::Draw()
{
    // Setup viewport and matrises
    // TODO: get window dimensions dynamically
    m_gfx->SetViewport(0, 0, 800, 600);
    
    if (m_Running)
    {
      if (m_RedrawNeeded)
      {
        //glClear(GL_DEPTH_BUFFER_BIT);
        if (m_RedrawStencil && !m_RedrawFull)
        {
          glEnable(GL_STENCIL_TEST);
          glDisable(GL_DEPTH_TEST);
          glClear(GL_STENCIL_BUFFER_BIT);
          
          glStencilFunc(GL_ALWAYS, 0x1, 0x1);
          glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
          
          m_StencilQB->Draw();
          m_StencilQB->Reset();
          m_RedrawStencil = false;
          
          glStencilFunc(GL_EQUAL, 0x1, 0x1);
          glStencilOp(GL_KEEP, GL_REPLACE, GL_KEEP);
          
          glEnable(GL_DEPTH_TEST);
        } else {
          glDisable(GL_STENCIL_TEST);
        }
        
        
        ResetDepth();
        glClear(GL_DEPTH_BUFFER_BIT);
        
        
        // Reset all quadbatches
        for(int i = 0; i < m_QuadBatchCount; ++i)
        {
          m_QuadBatches[i]->Reset();
        }
        
        CallScriptFunc("_draw");
        
        
        // Close last used QB
        if (m_QuadBatchCurrent >= 0)
        {
          m_QuadBatches[m_QuadBatchCurrent]->End();
        }
        
        // Draw all quadbatches
        for(int i = 0; i < m_QuadBatchCount; ++i)
        {
          m_QuadBatches[i]->Draw();
        }
        
        m_win->Swap();
        
        m_RedrawFull = false;
        m_RedrawNeeded = false;
        m_QuadBatchCurrent = -1;
        m_TransformMatrix = Math::Mat4::Identity;
      }  
    } else {
      Math::Mat4 prjmat = Math::Mat4::Ortho(-400, 400, 300, -300, -1000.0f, 1000.0f);
      m_gfx->SetProjection(&prjmat);
      glClearColor(46.0f/255.0f,46.0f/255.0f,46.0f/255.0f,1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
      // Display application error
      m_AppErrorQB->Draw();
      
      m_win->Swap();
    }
}

bool LuaApp::HandleErrors(int _error)
{
  if (_error != 0)
	{
		if (_error == LUA_ERRRUN)
		{
		  // Call error function in bootstrap.lua
      const char* _errmsg = lua_tostring(L, -1);
      
			// Push error handling function
      lua_getglobal(L, "debug");
    	lua_getfield(L, -1, "traceback");
    	lua_remove(L, -2);

    	// set function name to be called
      lua_getglobal(L, "_runtimeerror");
      
      // push error msg
      lua_pushstring(L, _errmsg);

      // Call function and wrap with error handler
      int s = lua_pcall(L, 1, 0, -3);
      if (s) {
        Message(LOCAL_MSG, "Error when calling runtime errror handler! %s", lua_tostring(L, -1));
        lua_pop(L, 1);
        m_Running = false;
      }
      
		} else {
		  if (_error == LUA_ERRMEM)
			  Message(LOCAL_MSG, "-- Script memory allocation error: --");
		  else
			  Message(LOCAL_MSG, "-- Fatal script error: --");
			  
		  Message(LOCAL_MSG, "%s", lua_tostring(L, -1));
  		lua_pop(L, 1); // remove error message
  		m_Running = false;
	  }
		return false;
	}
	return true;
}

bool LuaApp::CallScriptFunc(const char* _funcname, int nargs)
{ 
  // Push error handling function
  lua_getglobal(L, "app");
	lua_getfield(L, -1, "traceback");
	lua_remove(L, -2);
	if (nargs > 0)
    lua_insert(L, -nargs);
	
	// set function name to be called
  lua_getglobal(L, _funcname);
  if (nargs > 0)
    lua_insert(L, -nargs);
  
  // Call function and wrap with error handler
  return HandleErrors(lua_pcall(L, nargs, 0, -2-nargs));
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
    luaopen_appcore(L);
    luaopen_appinput(L);
    luaopen_appgraphics(L);
	luaopen_appsound(L);
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


