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

#include "BlockRenderer.h"

#include <ctime>

#include "Camera.h"

using namespace Pxf;
using namespace Math;


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

	int tick_id = snd->RegisterSound("data/tick.ogg");
	Resource::Font* fnt = res->Acquire<Resource::Font>("data/Monaco12p.pfnt");

	Derp::AuxiliaryBlock block;
	Derp::RenderBlock rblock;
	Derp::PostProcessBlock pblock;
	Graphics::Texture* tex = pblock.GetOutputValue(0);

	Graphics::WindowSpecifications spec;
	spec.Width = settings["video"].get("width", 800).asInt();
	spec.Height = settings["video"].get("height", 600).asInt();
	spec.ColorBits = 24;
	spec.AlphaBits = 8;
	spec.DepthBits = 8;
	spec.StencilBits = 0;
	spec.FSAASamples = 0;
	spec.Fullscreen = false;
	spec.Resizeable = false;
	spec.VerticalSync = settings["video"].get("vsync", true).asBool();
	
	Graphics::Window* win = gfx->OpenWindow(&spec);
	Graphics::Model* test_model = gfx->CreateModel("data/teapot.ctm");

	gluPerspective(45.0f,800/600,1.0f,20000.0f);

	SimpleCamera cam;
	cam.SetPerspective(45.0f,800 / 600, 1.0f,10000.0f);
	cam.SetLookAt(0.0f,0.0f,0.0f);
	cam.SetPosition(0.0f,0.0f,10.0f);

	float a = 0.0f;
	float mousesens = 2.0f;

	int oldmx,oldmy;
	float cam_z = 15.0f;

	Timer t;
	while(win->IsOpen())
	{
		t.Start();
		glMatrixMode(GL_PROJECTION);
		//glPushMatrix();

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		inp->Update();

		gluLookAt(0.0f,0.0f,cam_z,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f); 

		if (inp->GetLastKey() == Input::ESC)
			break;


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

		a += 0.1f;
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
		
		glRotatef(a,1.0f, 0, 0);
		//glScalef(0.1, 0.1, 0.1);
		test_model->Draw();

		//
		//gfx->DrawBuffer(pBuff,24);

		/*
		gfx->Translate(Pxf::Math::Vec3f(0.0f,0.0f,-20000.0f));

		glRotatef(0.1f,0.0f,0.0f,1.0f);
		test_model->Draw();

		gfx->Translate(Pxf::Math::Vec3f(0.0f,0.0f,20000.0f)); */


		oldmx = mx; oldmy = my;
		
		if (inp->GetLastButton() == Input::MOUSE_LEFT)
			snd->Play(tick_id);
		if (inp->GetLastButton() == Input::MOUSE_RIGHT)
			snd->Pause(tick_id);

		inp->ClearLastKey();
		inp->ClearLastButton();

		char title[512];
		
		t.Stop();
		Format(title, "Renderer (fps: %d, processing time: %d ms)", win->GetFPS(), t.Interval());
		win->SetTitle(title);
		win->Swap();

		glPopMatrix();
	}
	

	delete kernel;
	return 0;
}

