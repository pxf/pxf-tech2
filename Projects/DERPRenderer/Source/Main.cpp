#include <stdio.h>

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Timer.h>

#include <Pxf/Audio/AudioDevice.h>
#include <Pxf/Input/InputDevice.h>
#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Graphics/Window.h>
#include <Pxf/Graphics/WindowSpecifications.h>
#include <Pxf/Graphics/RenderBuffer.h>
#include <Pxf/Graphics/Model.h>

#include <Pxf/Graphics/VertexBuffer.h>
#include <Pxf/Graphics/VertexBufferDefs.h>

#include <Pxf/Base/Hash.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Memory.h>
#include <Pxf/Base/Random.h>

#include <Pxf/Modules/pri/OpenGL.h>

#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/ResourceLoader.h>
#include <Pxf/Resource/Image.h>
#include <Pxf/Resource/Json.h>
#include <Pxf/Resource/Sound.h>
#include <Pxf/Resource/Font.h>
#include <Pxf/Resource/Text.h>

#include "Renderer.h"
#include "SimpleQuad.h"

#include <ctime>

#include "Camera.h"

using namespace Pxf;
using namespace Math;
using namespace Graphics;
using namespace Derp;


int main()
{
	Pxf::RandSetSeed(time(NULL));
	Kernel* kernel = Pxf::Kernel::GetInstance();

	// Just load everything
	kernel->RegisterModule("pri", 0xFFFF, true);
	kernel->RegisterModule("img", 0xFFFF, true);
	kernel->RegisterModule("mesh", 0xFFFF, true);
	kernel->RegisterModule("snd", 0xFFFF, true);
	kernel->RegisterModule("json", 0xFFFF, true);
	kernel->RegisterModule("net", 0xFFFF, true);
	kernel->DumpAvailableModules();

	Graphics::GraphicsDevice* gfx = kernel->GetGraphicsDevice();
	Audio::AudioDevice* snd = kernel->GetAudioDevice();
	Input::InputDevice* inp = kernel->GetInputDevice();
	Resource::ResourceManager* res = kernel->GetResourceManager();
	res->DumpResourceLoaders();

	// load settings
	Resource::Json* jdoc = res->Acquire<Resource::Json>("data/config.json");
	Json::Value settings;
	if (jdoc)
		settings = jdoc->GetRoot();
	else
	{
		settings["audio"]["buffersize"] = 1024;
		settings["audio"]["max_voices"] = 8;
		settings["input"]["mouse_accel"] = 0.9f;
		settings["video"]["width"] = 800;
		settings["video"]["height"] = 600;
		settings["video"]["vsync"] = false;

		Resource::JsonLoader* jld = res->FindResourceLoader<Resource::JsonLoader>("json");
		if (jld)
		{
			jdoc = jld->CreateEmpty();
			jdoc->SetRoot(settings);
			jdoc->SaveToDisk("data/config.json");
			Message("Main", "Saving new config!");
		}
	}

	snd->Initialize(settings["audio"].get("buffersize", 512).asUInt()
				   ,settings["audio"].get("max_voices", 8).asUInt());
	
	Derp::Renderer* renderer = new Derp::Renderer("data/testblocks.json");
	renderer->LoadJson();
	
	Graphics::WindowSpecifications spec;
	spec.Width = renderer->m_Width;//settings["video"].get("width", 800).asInt();
	spec.Height = renderer->m_Height;//settings["video"].get("height", 600).asInt();
	spec.ColorBits = 24;
	spec.AlphaBits = 8;
	spec.DepthBits = 8;
	spec.StencilBits = 0;
	spec.FSAASamples = 0;
	spec.Fullscreen = false;
	spec.Resizeable = false;
	spec.VerticalSync = settings["video"].get("vsync", true).asBool();
	Graphics::Window* win = gfx->OpenWindow(&spec);
	
	// Build pipeline graph
	renderer->BuildGraph();
	
	// Setup full screen quad
	SimpleQuad* finalquad = new SimpleQuad(0.0f, 0.0f, 1.0f, 1.0f);

	/*Graphics::RenderBuffer* a = gfx->CreateRenderBuffer(GL_RGBA8,256,256);
	Graphics::RenderBuffer* b = gfx->CreateRenderBuffer(GL_RGBA8,256,256);

	Graphics::FrameBufferObject* FBO = gfx->CreateFrameBufferObject();
	FBO->Attach(a,GL_COLOR_ATTACHMENT0);
	FBO->Attach(b,GL_COLOR_ATTACHMENT1);*/

	Timer t;
	while(win->IsOpen())
	{
		t.Start();	

		// Execute renderer/pipeline
		renderer->Execute();
			
		// TODO: Setup normal backbuffer and render final output
		
		// Setup ogl
		gfx->SetViewport(0, 0, win->GetWidth(), win->GetHeight());
	  //Math::Mat4 prjmat = Math::Mat4::Ortho(0, win->GetWidth(), win->GetHeight(), 0, 1.0f, 10000.0f);
		Math::Mat4 prjmat = Math::Mat4::Ortho(0, 1.0f, 1.0f, 0, -1.0f, 10000.0f);
	  gfx->SetProjection(&prjmat);
	  glClearColor(26.0f/255.0f,26.0f/255.0f,26.0f/255.0f,1.0f);
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Render final output
		glEnable(GL_TEXTURE_2D);
		gfx->BindTexture(renderer->GetResult());
		finalquad->Draw();
		
		// Pick out pipeline result and render it!
		
		/*
		
		glMatrixMode(GL_PROJECTION);
		//glPushMatrix();

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		inp->Update();

		//gluLookAt(0.0f,0.0f,cam_z,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f); 
		

		if (inp->GetLastKey() == Input::ESC)
			break;

		/*
		if (inp->GetLastKey() == Input::UP)
		{
			cam_z += 0.5f;
			printf("%f\n", cam_z);
		}
		if (inp->GetLastKey() == Input::DOWN)
		{
			cam_z -= 0.5f;
			printf("%f\n", cam_z);
		}
		
		int mx,my;
		inp->GetMousePos(&mx,&my);
		

		//glEnable(GL_DEPTH_TEST);

		// test
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		gfx->SetViewport(0,0,800,600);

		//a += 0.1f;
		//cam.SetPosition(0.0f,a,0.0f);

		glClearColor(46.0f/255.0f,46.0f/255.0f,46.0f/255.0f,1.0f);

		//cam.SetRotation((mx - oldmx) / mousesens ,  (my - oldmy) / mousesens, 0.0f);
		//gfx->SetProjection(cam.GetProjectionView());
		//gfx->SetModelView(cam.GetModelView());

		/*
		glBegin(GL_TRIANGLES);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(1.0f,0.0f,0.0f);
		glVertex3f(0.0f,1.0f,0.0f);
		glEnd(); */
		
		//glRotatef(a,1.0f, 0, 0);
		//glScalef(0.1, 0.1, 0.1);
		//test_model->Draw();

		//
		//gfx->DrawBuffer(pBuff,24);

		/*
		gfx->Translate(Pxf::Math::Vec3f(0.0f,0.0f,-20000.0f));

		glRotatef(0.1f,0.0f,0.0f,1.0f);
		test_model->Draw();

		gfx->Translate(Pxf::Math::Vec3f(0.0f,0.0f,20000.0f)); */


		/*oldmx = mx; oldmy = my;
		
		if (inp->GetLastButton() == Input::MOUSE_LEFT)
			snd->Play(tick_id);
		if (inp->GetLastButton() == Input::MOUSE_RIGHT)
			snd->Pause(tick_id);
		
		inp->ClearLastKey();
		inp->ClearLastButton();
		*/
		
		inp->Update();
		
		if (inp->GetLastKey() == Input::ESC)
			break;

		if (inp->GetLastKey() == Input::F5)
		{
			win->Swap();
			//Resource::Image* img = gfx->CreateImageFromTexture(renderer->GetResult());
			Texture* tex = gfx->CreateTextureFromFramebuffer();
			Resource::Image* img = gfx->CreateImageFromTexture(tex);
			gfx->DestroyTexture(tex);
			img->SaveAs("data/screenshot.tga");
			delete img;
		}
		
		inp->ClearLastKey();
		inp->ClearLastButton();
		
		char title[512];
		t.Stop();
		Format(title, "Renderer (fps: %d, processing time: %d ms)", win->GetFPS(), t.Interval());
		win->SetTitle(title);
		win->Swap();

		//glPopMatrix();
	}
	
	delete finalquad;
	delete renderer;

	delete kernel;
	return 0;
}

