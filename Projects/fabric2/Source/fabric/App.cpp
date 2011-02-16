#include "App.h"

#include <Pxf/Modules/pri/OpenGL.h>

#include "AppCoreLib.h"
#include "AppInputLib.h"
#include "AppGraphicsLib.h"
#include "AppSoundLib.h"
#include "AppUtilsLib.h"

#include <Pxf/Audio/AudioDevice.h>
#include <Pxf/Base/String.h>
#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/Text.h>

#define LOCAL_MSG "Fabric"

using namespace Fabric;
using namespace Pxf;
using namespace Pxf::Graphics;

static App* _appinstance;

Pxf::Timer g_TimerRuntime;

App* App::GetInstance()
{
    return _appinstance;
}

App::App(Graphics::Window* _win, const char* _filepath)
	: m_snd(0)
{
    m_Filepath = _filepath;
    m_win = _win;
    
    m_CurrentDepth = 0.0f;
    m_CurrentColor = Math::Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
    
    m_AppErrorQB = new TexturedQuadBatch(4, "fabric/apperror.png", &m_CurrentDepth, &m_CurrentColor, NULL);
    m_AppErrorQB->Begin();
    m_AppErrorQB->AddCentered(0, 0, 512, 256);
    m_AppErrorQB->End();
    
    m_StencilQB = new QuadBatch(128*4, &m_CurrentDepth, &m_CurrentColor);
    
    m_RedrawNeeded = false;
    m_RedrawStencil = false;
    m_RedrawFull = false;
    m_RedrawMode = FABRIC_REDRAWMODE_FULL;

		m_Dirty = false;
		m_UsingFBO = false;
		
		m_SuccessFlushCount = 0;
		m_FlushCount = 0;
		m_QuadCount = 0;
    
    m_Started = false;
    m_Running = false;
    m_Shutdown = false;
    m_Reboot = false;
    m_QuadBatchCurrent = -1;
    m_QuadBatchCount = 0;
    
    m_TransformMatrix = Math::Mat4::Identity;
    
    // get engine system pointers for easy access later on
    m_gfx = Kernel::GetInstance()->GetGraphicsDevice();
    m_inp = Kernel::GetInstance()->GetInputDevice();
    m_snd = Kernel::GetInstance()->GetAudioDevice();
	m_net = Kernel::GetInstance()->GetNetworkDevice();
    
	m_snd->Initialize();

    // Set "snigelton"
    _appinstance = this;
    
    L = NULL;
}

App::~App()
{
    CleanUp();
    
    delete m_AppErrorQB;
    delete m_StencilQB;
}

void App::Init()
{
  m_RedrawMode = FABRIC_REDRAWMODE_FULL;
  
  m_MaxQuadCount = 1024*1024;//2048*2;
  m_DepthStep = (FABRIC_DEPTH_RANGE / m_MaxQuadCount);
  
  m_QuadBatches[m_QuadBatchCount] = new QuadBatch(m_MaxQuadCount, &m_CurrentDepth, &m_CurrentColor, &m_TransformMatrix);
  m_QuadBatchCount++;

	m_UsingFBO = false;
  
  // Init GL settings
  //Math::Mat4 prjmat = Math::Mat4::Ortho(0, m_win->GetWidth(), m_win->GetHeight(), 0, FABRIC_DEPTH_FAR, FABRIC_DEPTH_NEAR);
  //m_gfx->SetProjection(&prjmat);
  
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

void App::CleanUp()
{
    // Close lua state
    if (L != NULL)
    {
      lua_close(L);
      L = NULL;
    }
  
    // reset states
    m_Started = false;
    m_Running = false;
    m_Reboot = false;
    
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
    
		// Clear special raw texture array
		for(Util::Array<TexturedQuadBatch*>::iterator iter = m_RawTextureQB.begin(); iter != m_RawTextureQB.end(); ++iter)
		{
			delete (*iter);
		}
		m_RawTextureQB.clear();
    
    // reset transform matrix
    m_TransformMatrix = Math::Mat4::Identity;
}

static int luaL_exec(lua_State* L, const char* filepath)
{
	Pxf::Resource::ResourceManager* res = Kernel::GetInstance()->GetResourceManager();
	Resource::Text* text = res->Acquire<Resource::Text>(filepath, "txt");
	if (text)
	{
		int s = luaL_loadstring(L, text->Ptr());
		res->Release(text);
		return s;
	}
	return 0;
}

bool App::Boot()
{
  // Init ourselves
  Init();

  g_TimerRuntime.Start();
  
  // Init lua state
  L = luaL_newstate();
  
  // Register lua libs
  _register_lua_libs_callbacks();
  
  // Register own callbacks
  _register_own_callbacks();
  
  ////////////////////////////////
  // Load bootstrap script
  //int s = luaL_loadfile(L, "fabric/boot.lua");
  int s = luaL_exec(L, "fabric/boot.lua");
  
  if (!s)
	{
		s = lua_pcall(L, 0, LUA_MULTRET, 0);
		if ( s ) {
			Message(LOCAL_MSG, " [Error while running bootstrap.lua] -- %s", lua_tostring(L, -1));
			lua_pop(L, 1); // remove error message
		} else {
      // Set app.instance to this class instance!
      // Instance will later be used to call correct app instance.
      /*lua_getglobal(L, FABRIC_TABLE);
      lua_pushlightuserdata(L, this);
      lua_setfield(L, -2, "instance");
      lua_pop(L, 1);*/
		    
      //////////////////////////
      // Load main app script   
      s = luaL_exec(L, m_Filepath);
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
		Message(LOCAL_MSG, "[Error while loading boot.lua] -- %s", lua_tostring(L,-1));
		lua_pop(L, 1);
	}
    
  return m_Running;
}

void App::Reboot()
{
  //CleanUp();
  //return Boot();
  m_Reboot = true;
}

void App::Shutdown()
{
  m_Shutdown = true;
}


bool App::Update()
{
  if (m_Reboot)
  {
    CleanUp();
    Boot();
  }
  
  m_TimerUpdate.Start();
    if (m_Running)
    {
	  	if (m_RedrawMode != FABRIC_REDRAWMODE_FULL)
      {
        //printf("pdate\n");
				//glfwDisable(GLFW_AUTO_POLL_EVENTS);
				//glfwSleep(0.01);
        //glfwPollEvents();
        //glfwWaitEvents();
		  }
      CallScriptFunc("_update");
    } else {
      // A application error has occurred, see if the user wants to reboot or quit
      int mx,my;
      m_inp->GetMousePos(&mx, &my);
      
      if (m_inp->IsButtonDown(Input::MOUSE_LEFT))
      {
        if (my > m_win->GetHeight()/2 + 34 &&
            my < m_win->GetHeight()/2 + 34 + 26)
        {
          // reboot button
          if (mx > m_win->GetWidth()/2 - 105 &&
              mx < m_win->GetWidth()/2 + 22)
              Reboot();
        
          // quit button
          if (mx > m_win->GetWidth()/2 + 52 &&
              mx < m_win->GetWidth()/2 + 108)
              m_Shutdown = true;
          
        }
      }
        
    }
    m_TimerUpdate.Stop();
    //Message(LOCAL_MSG, "update() : %ims", m_TimerUpdate.Interval());
    
    return !m_Shutdown;
}

int App::ChangeActiveQB(unsigned int _id)
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

QuadBatch* App::GetActiveQB()
{
  if (m_QuadBatchCurrent < 0)
    return NULL;
  
  return m_QuadBatches[m_QuadBatchCurrent];
}

void App::IncDepth()
{
  m_CurrentDepth = m_CurrentDepth - m_DepthStep;
}

void App::ResetDepth()
{
  m_CurrentDepth = FABRIC_DEPTH_FAR;
}

void App::Redraw()
{
  m_RedrawNeeded = true;
  m_RedrawFull = true;
  m_RedrawStencil = false;
}

void App::Redraw(int x, int y, int w, int h)
{
  m_RedrawNeeded = true;
  
  if (m_RedrawMode == FABRIC_REDRAWMODE_FULL)
  {
    m_RedrawFull = true;
  }
  
  if (!m_RedrawFull)
  {
    m_RedrawStencil = true;
  
    m_StencilQB->Begin();
    m_StencilQB->AddTopLeft(x,y,w,h);
    m_StencilQB->End();
  }
}

void App::Flush()
{
	// Flush (draw/show) current state
	if (m_Dirty)
	{
		// Close last used QB
		int t_ActiveQB = m_QuadBatchCurrent;
		if (m_QuadBatchCurrent >= 0)
		{
			m_QuadBatches[m_QuadBatchCurrent]->End();
		}
		
		int num_passes = 1;
		//int num_passes = 2;
		if (m_UsingFBO)
			num_passes = 1;
		
		glClear(GL_DEPTH_BUFFER_BIT);
		
		for (int renderpass = 0; renderpass < num_passes; ++renderpass)
		{
			/*if (m_RedrawStencil && !m_RedrawFull)
			{
				glEnable(GL_STENCIL_TEST);
				glDisable(GL_DEPTH_TEST);
				glClear(GL_STENCIL_BUFFER_BIT);
				
				glStencilFunc(GL_ALWAYS, 0x1, 0x1);
				glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
				
				m_StencilQB->Draw();
				
				glStencilFunc(GL_EQUAL, 0x1, 0x1);
				glStencilOp(GL_KEEP, GL_REPLACE, GL_KEEP);

				glEnable(GL_DEPTH_TEST);
			} else {*/
				glDisable(GL_STENCIL_TEST);
			//}
			
			// Draw all quadbatches
			for (int i = 0; i < m_QuadBatchCount; ++i)
			{
				// Count quads
				m_QuadCount += m_QuadBatches[i]->GetQuadCount();
				
				// Draw!
			  m_QuadBatches[i]->Draw();
			}

			// Draw all special raw texuters
			for(Util::Array<TexturedQuadBatch*>::iterator iter = m_RawTextureQB.begin(); iter != m_RawTextureQB.end(); ++iter)
			{
				// Count quads
				m_QuadCount += (*iter)->GetQuadCount();
				
				// Draw!
				(*iter)->Draw();
			}
			
			/*if (!m_UsingFBO)
				m_win->Swap();*/
		}
		
		// The active state is not 'dirty' anymore!
		m_Dirty = false;
		
		// Reset all quadbatches
		for(int i = 0; i < m_QuadBatchCount; ++i)
		{
			m_QuadBatches[i]->Reset();
		}
		
		// reset all special raw texuters
		for(Util::Array<TexturedQuadBatch*>::iterator iter = m_RawTextureQB.begin(); iter != m_RawTextureQB.end(); ++iter)
		{
			(*iter)->Reset();
		}
		
		// Set active QB to the previously active one
		if (t_ActiveQB >= 0)
			ChangeActiveQB(t_ActiveQB);//m_QuadBatchCurrent = t_ActiveQB;
			
		// Increase successfull flush count
		m_SuccessFlushCount += 1;
		ResetDepth();
		
	} else {
		//Message(LOCAL_MSG, "Trying to flush non-dirty state!");
	}
	
	m_FlushCount += 1;
	
}

void App::Draw()
{
	m_TimerDraw.Start();
	// Setup viewport and matrises
	m_gfx->SetViewport(0, 0, m_win->GetWidth(), m_win->GetHeight());
	Math::Mat4 prjmat = Math::Mat4::Ortho(0, m_win->GetWidth(), m_win->GetHeight(), 0, FABRIC_DEPTH_FAR, FABRIC_DEPTH_NEAR);
	m_gfx->SetProjection(&prjmat);
    
	if (m_Running)
	{
		if (m_RedrawNeeded)
		{
			// Reset stats
			m_SuccessFlushCount = 0;
			m_FlushCount = 0;
			m_QuadCount = 0;
			
			// Reset draw depth
			ResetDepth();
			
			// Reset all quadbatches
			for(int i = 0; i < m_QuadBatchCount; ++i)
			{
				m_QuadBatches[i]->Reset();
			}
			// reset all special raw texuters
			for(Util::Array<TexturedQuadBatch*>::iterator iter = m_RawTextureQB.begin(); iter != m_RawTextureQB.end(); ++iter)
			{
				(*iter)->Reset();
			}
		
			CallScriptFunc("_draw");
			Flush();
			m_win->Swap();
			
			m_QuadBatchCurrent = -1;
			m_TransformMatrix = Math::Mat4::Identity;
			m_RedrawFull = false;
			m_RedrawNeeded = false;
			m_RedrawStencil = false;
			
			m_StencilQB->Reset();
 
		}  
	} else {
		Math::Mat4 prjmat = Math::Mat4::Ortho(-m_win->GetWidth()/2, m_win->GetWidth()/2, m_win->GetHeight()/2, -m_win->GetHeight()/2, -1000.0f, 1000.0f);
		m_gfx->SetProjection(&prjmat);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Display application error
		m_AppErrorQB->Draw();

		m_win->Swap();
	}
	    
	m_TimerDraw.Stop();
	//Message(LOCAL_MSG, "draw() : %ims", m_TimerDraw.Interval());
}

bool App::GuiHit()
{
	lua_getglobal(L, "__guihit");
	bool guihit = lua_toboolean(L, -1);
	lua_pop(L, 1);
	return guihit;
}

bool App::HandleErrors(int _error)
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

bool App::CallScriptFunc(const char* _funcname, int nargs)
{ 
  // Push error handling function
  lua_getglobal(L, "debug");
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

void App::BindExternalFunction(const char* _funcname, lua_CFunction _funcp)
{
	m_ExternalFuncs.insert( std::make_pair(_funcname, _funcp));
}


///////////////////////////////////////////////////////////////////
// Private methods
void App::_register_lua_libs_callbacks()
{
	// Lua libs
	/*static const luaL_Reg lualibs[] = {
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
	}*/
  luaL_openlibs(L);
}

int pxf_loader (lua_State *_L)
{
    std::string path = luaL_checkstring(_L, 1);
	printf("Loading: %s\n", path.c_str());
    if (!Pxf::IsSuffix(path.c_str(), ".lua"))
        path += ".lua";
    Pxf::Resource::ResourceManager* res = Kernel::GetInstance()->GetResourceManager();
	if (res->HasCachedFile(path.c_str()))
	{
		Resource::Text* text = res->Acquire<Resource::Text>(path.c_str(), "txt");
		if (text)
		{
			int size = text->Length();
			const char* data = text->Ptr();
			luaL_loadbuffer(_L, data, size-1, path.c_str());
			res->Release(text);
			return 1;
		}
	}

    std::string err = "Error loading file: " + path;
    lua_pushstring(_L, err.c_str());
	lua_error(_L);
	return 0;

}

static void register_resource_loader(lua_State* _L)
{
/*
	// get table.insert
    lua_getglobal(L, "table");
    lua_getfield(L, -1, "insert");
    lua_remove(L, -2);

    // get package.loaders
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "loaders");
    lua_remove(L, -2);

    // insert physfs loader
    lua_pushvalue(L, -2);
    lua_pushvalue(L, -2);
    lua_pushnumber(L, 2);
    lua_pushcfunction(L, pxf_loader);
    lua_call(L, 3, 0);

    lua_pop(L, 2);
*/

    lua_getfield(_L, LUA_GLOBALSINDEX, "package");
	lua_getfield(_L, -1, "loaders");
	lua_remove(_L, -2);
	int loader_count = 0;
    lua_pushnil(_L);
    while (lua_next(_L, -2) != 0)
    {
        lua_pop(_L, 1);
        loader_count++;
    }
    lua_pushinteger(_L, loader_count + 1);
    lua_pushcfunction(_L, pxf_loader);
    lua_rawset(_L, -3);
	lua_pop(_L, 1);

}

void App::_register_own_callbacks()
{
	register_resource_loader(L);

	// Register own callbacks
	lua_register(L, "print", Print);
	lua_register(L, "uptime", Uptime);
    
	// Create empty luagame table
	lua_newtable(L);
	lua_setglobal (L, FABRIC_TABLE);
   
	// Register subsystems
	luaopen_appcore(L);
	luaopen_appinput(L);
	luaopen_appgraphics(L);
	luaopen_appsound(L);
	luaopen_apputils(L);
	
	// Iterate all external functions
	for(Util::Map<const char*, lua_CFunction>::iterator iter = m_ExternalFuncs.begin(); iter != m_ExternalFuncs.end(); ++iter)
	{
		lua_pushcfunction(L, (*iter).second);
		lua_setglobal(L, (*iter).first);
	}

	// Sockets
	//luaopen_socket_core(L);
}


///////////////////////////////////////////////////////////////////
// Callback methods
int App::Uptime(lua_State *_L)
{
	int n = lua_gettop(_L);
	if (n != 0)
	{
		lua_pushstring(_L, "Uptime function takes no arguments.");
		lua_error(_L);
	}
	else
	{
		double time = g_TimerRuntime.Interval() / 1000.f;
		lua_pushnumber(_L, time);
		return 1;
	}
	return 0;
}

int App::Print(lua_State *_L)
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
        lua_pcall(_L, 1, 1, 0);
        s = lua_tostring(_L, -1);  /* get result */
        if (s == NULL)
            return luaL_error(_L, LUA_QL("tostring") " must return a string to "
                                  LUA_QL("print"));
        if (i>1)
            fputs("\n   -   > ", stdout);
        else
						fputs("fabric > ", stdout);
        fputs(s, stdout);
        lua_pop(_L, 1);  /* pop result */
    }
    fputs("\n", stdout);
    return 0;
}
