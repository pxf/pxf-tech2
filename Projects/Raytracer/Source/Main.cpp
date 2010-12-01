#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Timer.h>
#include <Pxf/Base/Logger.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Random.h>
#include <Pxf/Base/Memory.h>
#include <Pxf/Math/Math.h>
#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Graphics/Window.h>
#include <Pxf/Graphics/WindowSpecifications.h>
#include <Pxf/Graphics/Texture.h>
#include <Pxf/Graphics/PrimitiveBatch.h>
#include <Pxf/Input/InputDevice.h>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include "Renderer.h"

#include "fabric/App.h"

#include <RaytracerClient.h>

using namespace Pxf;
using namespace Graphics;
using namespace Math;

int test_cb(lua_State* L)
{
	lua_pushstring(L, "sup?");
	return 1;
}

int main(int argc, char* argv[])
{
	Pxf::RandSetSeed(time(NULL));
	Kernel* kernel = Pxf::Kernel::GetInstance();
	kernel->RegisterModule("pri", 0xFFFFFFFF, true);
	kernel->RegisterModule("img", Pxf::System::SYSTEM_TYPE_RESOURCE_LOADER, true);
	
	Graphics::GraphicsDevice* gfx = kernel->GetGraphicsDevice();
	Input::InputDevice* inp = kernel->GetInputDevice();
	
	
	Graphics::WindowSpecifications spec;
	spec.Width = 512;
	spec.Height = 512;
	spec.ColorBits = 24;
	spec.AlphaBits = 8;
	spec.DepthBits = 8;
	spec.StencilBits = 0;
	spec.FSAASamples = 0;
	spec.Fullscreen = false;
	spec.Resizeable = false;
	spec.VerticalSync = true;
	Graphics::Window* win = gfx->OpenWindow(&spec);
	
	// Generate awesome red output buffer
	const int w = 128;
	const int h = 128;
	const int channels = 3;
	const int task_count = 16;
	int task_size_w = w / task_count;
	int task_size_h = h / task_count;
	char pixels[w*h*channels];
	
	// job specifics
	batch_blob_t blob;
	blob.pic_w = w;
	blob.pic_h = h;
	blob.samples_per_pixel = 10; // 10 -> 10*10 = 100
	blob.bounce_count = 4; // Number of reflection bounces
	blob.interleaved_feedback = 2;
	
	// add a couple of primitives to the data blob
	material_t plane_mat_white,plane_mat_red,plane_mat_green,sphere_mat1,sphere_mat2;
	plane_mat_white.ambient = Vec3f(0.1f, 0.1f, 0.1f);
	plane_mat_white.diffuse = Vec3f(1.0f, 1.0f, 1.0f);
	plane_mat_white.reflectiveness = 0.3f;
	plane_mat_red.ambient = Vec3f(0.1f, 0.0f, 0.0f);
	plane_mat_red.diffuse = Vec3f(1.0f, 0.0f, 0.0f);
	plane_mat_red.reflectiveness = 0.3f;
	plane_mat_green.ambient = Vec3f(0.0f, 0.1f, 0.0f);
	plane_mat_green.diffuse = Vec3f(0.0f, 1.0f, 0.0f);
	plane_mat_green.reflectiveness = 0.3f;
	
	sphere_mat1.ambient = Vec3f(0.1f, 0.0f, 0.0f);
	sphere_mat1.diffuse = Vec3f(1.0f, 0.8f, 0.8f);
	sphere_mat1.reflectiveness = 0.7f;
	sphere_mat2.ambient = Vec3f(0.1f, 0.1f, 0.1f);
	sphere_mat2.diffuse = Vec3f(1.0f, 1.0f, 1.0f);
	sphere_mat2.reflectiveness = 1.0f;
	
	blob.prim_count = 0;
	blob.primitives[blob.prim_count++] = new Plane(Pxf::Math::Vec3f(0.0f, -5.0f, 0.0f), Pxf::Math::Vec3f(0.0f, 1.0f, 0.0f), plane_mat_white); // bottom
	blob.primitives[blob.prim_count++] = new Plane(Pxf::Math::Vec3f(0.0f, 5.0f, 0.0f), Pxf::Math::Vec3f(0.0f, -1.0f, 0.0f), plane_mat_white); // top
	blob.primitives[blob.prim_count++] = new Plane(Pxf::Math::Vec3f(-5.0f, 0.0f, 0.0f), Pxf::Math::Vec3f(1.0f, 0.0f, 0.0f), plane_mat_red); // left
	blob.primitives[blob.prim_count++] = new Plane(Pxf::Math::Vec3f(5.0f, 0.0f, 0.0f), Pxf::Math::Vec3f(-1.0f, 0.0f, 0.0f), plane_mat_green); // right
	blob.primitives[blob.prim_count++] = new Plane(Pxf::Math::Vec3f(0.0f, 0.0f, 10.0f), Pxf::Math::Vec3f(0.0f, 0.0f, -1.0f), plane_mat_white); // back
	blob.primitives[blob.prim_count++] = new Sphere(Pxf::Math::Vec3f(-2.0f, -3.0f, 6.0f), 1.5f, sphere_mat1);
	blob.primitives[blob.prim_count++] = new Sphere(Pxf::Math::Vec3f(2.0f, 0.0f, 8.0f), 2.0f, sphere_mat2);

	/*
	// Add 64 spheres on the floor, should slow down the render a bit. Compare with kd-tree.
	/*for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			blob.primitives[blob.prim_count++] = new Sphere(Math::Vec3f(x-3.5f,-4.f,y+2), .5f, (x+y)%2 == 0 ? sphere_mat1 : sphere_mat2);
		}
	}*/

	//blob.prim_count = 7;
	
	// generate a couple of random samples
	srand ( time(NULL) );
	for(int i = 0; i < 256; ++i)
	{
		blob.samples[i] = Pxf::RandFP32();
		//Pxf::Message("rand", "%d -> %f", i, blob.samples[i]);
	}
	
	// add a couple of lights to the data blob
	material_t light_mat1,light_mat2;
	light_mat1.diffuse = Vec3f(1.0f, 1.0f, 1.0f);
	light_mat2.diffuse = Vec3f(1.0f, 1.0f, 1.0f);
	//blob.lights[0] = new PointLight(Pxf::Math::Vec3f(0.0f, 4.8f, 5.0f), light_mat1);
	blob.lights[0] = new AreaLight(Pxf::Math::Vec3f(0.0f, 4.8f, 5.0f), 1.0f, 1.0f, Pxf::Math::Vec3f(0.0f, -1.0f, 0.0f), Pxf::Math::Vec3f(1.0f, 0.0f, 0.0f), 3, 5.0f, light_mat1);
	//blob.lights[1] = new AreaLight(Pxf::Math::Vec3f(0.0f, -4.8f, 5.0f), 1.0f, 1.0f, Pxf::Math::Vec3f(0.0f, -1.0f, 0.0f), Pxf::Math::Vec3f(1.0f, 0.0f, 0.0f), 9, light_mat1);
	blob.light_count = 1;
	
	// create textures and primitive batches
	Texture *region_textures[task_count*task_count] = {0};
	Texture *unfinished_task_texture = Pxf::Kernel::GetInstance()->GetGraphicsDevice()->CreateTexture("unfinished.png");
	unfinished_task_texture->SetMagFilter(TEX_FILTER_NEAREST);
	unfinished_task_texture->SetMinFilter(TEX_FILTER_NEAREST);

	int ty = 0;
	int tx = 0;
	int total_done = 0;
	
	PrimitiveBatch *pbatch = new PrimitiveBatch(Pxf::Kernel::GetInstance()->GetGraphicsDevice());

	// Fabric/GUI stuff
	Fabric::App* app = new Fabric::App(win, "fabric/main.lua");
	app->BindExternalFunction("testcb", test_cb);
	app->Boot();
	bool running = true;
	

	// Raytracer client test
	//------------------------
	RaytracerClient client(kernel);
	client.run_noblock();

	// add a bunch of tasks
	for(int y = 0; y < task_count; y++)
	{
		for(int x = 0; x < task_count; x++)
		{
			TaskRequest* req = new TaskRequest;
			req->blob = &blob;
			req->rect.x = x * task_size_w;
			req->rect.y = y * task_size_h;
			req->rect.h = task_size_h;
			req->rect.w = task_size_w;
			client.push_request(req);
		}
	}

	//client.wait();
	//------------------------


	Pxf::Timer render_timer;
	render_timer.Start();

	bool is_done = false;
	while(win->IsOpen() && running)
	{
		inp->Update();
		if (inp->GetLastKey() == Input::ESC)
			break;

		if (inp->GetLastButton() == Input::MOUSE_1)
		{
			int x, y;
			inp->GetMousePos(&x, &y);
			client.focus_renderer(x, y);
		}
			
		// Setup view!!!!!!!!
		gfx->SetViewport(0, 0, win->GetWidth(), win->GetHeight());
		Math::Mat4 prjmat = Math::Mat4::Ortho(0, w, h, 0, -0.1f, 100.0f);
		gfx->SetProjection(&prjmat);

		// Get results
		//-------------------------------------
		if(client.has_results())
		{
			TaskResult* res = client.pop_result();
			int x = res->rect.x / task_size_w;
			int y = res->rect.y / task_size_h;

			unsigned int idx = y*task_count+x;
			Pxf::Graphics::GraphicsDevice* gfx = Pxf::Kernel::GetInstance()->GetGraphicsDevice();
			region_textures[idx] = gfx->CreateTextureFromData((const unsigned char*)res->pixels, task_size_w, task_size_w, channels);
			region_textures[idx]->SetMagFilter(TEX_FILTER_NEAREST);
			region_textures[idx]->SetMinFilter(TEX_FILTER_NEAREST);
			if (res->final)
				total_done += 1;
		}

		// Draw
		for(int y = 0; y < task_count; y++)
		{
			for(int x = 0; x < task_count; x++)
			{
				unsigned int idx = y*task_count+x;
				if (region_textures[idx] == 0)
					Pxf::Kernel::GetInstance()->GetGraphicsDevice()->BindTexture(unfinished_task_texture);
				else
					Pxf::Kernel::GetInstance()->GetGraphicsDevice()->BindTexture(region_textures[idx]);

				// Setup quad
				pbatch->QuadsBegin();
				pbatch->QuadsDrawTopLeft(x*task_size_w, y*task_size_h, task_size_w, task_size_w);
				pbatch->QuadsEnd();
			}
		}
		//--------------------------------------

		if (total_done == task_count*task_count && !is_done)
		{
			//thread_executor.cancel();
			is_done = true;
			render_timer.Stop();
			char title[512];
			Format(title, "Render time: %d ms", render_timer.Interval());
			win->SetTitle(title);
		}
		
		running = app->Update();
		app->Draw();
		
		inp->ClearLastKey();
		inp->ClearLastButton();
		win->Swap();
	}

	client.cancel();
	client.wait();
	
	delete app;
	delete pbatch;

	return 0;
}

