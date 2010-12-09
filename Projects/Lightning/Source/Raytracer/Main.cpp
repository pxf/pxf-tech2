#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Timer.h>
#include <Pxf/Base/Logger.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Random.h>
#include <Pxf/Base/Memory.h>
#include <Pxf/Base/Platform.h>
#include <Pxf/Math/Math.h>
#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Graphics/Window.h>
#include <Pxf/Graphics/WindowSpecifications.h>
#include <Pxf/Graphics/Texture.h>
#include <Pxf/Graphics/PrimitiveBatch.h>
#include <Pxf/Graphics/Model.h>
#include <Pxf/Resource/Mesh.h>

#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/ResourceLoader.h>

#include <Pxf/Input/InputDevice.h>
#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/Mesh.h>
#include <Pxf/Modules/pri/OpenGL.h>

#include <ctime>
#include <cstdio>
#include <cstdlib>
#include "Renderer.h"
#include "Camera.h"

#include "connectionmanager.h"
#include "lightning.h"
#include "lightning.pb.h"
#include "client.pb.h"
#include "raytracer.pb.h"

#include "fabric/App.h"

#include <RaytracerClient.h>

using namespace Pxf;
using namespace Graphics;
using namespace Math;

int total_done = 0;
int total_count = 0;
Pxf::Timer render_timer;

// sup?
batch_blob_t blob;
ConnectionManager *cman;

// task/batch specific globals
const int w = 128;
const int h = 128;
const int channels = 3;
const int task_count = 8;
int task_size_w = w / task_count;
int task_size_h = h / task_count;

raytracer::DataBlob* gen_packet_from_blob(batch_blob_t* blob)
{
	raytracer::DataBlob* npack = new raytracer::DataBlob();
	npack->set_prim_count(blob->prim_count);
	npack->set_light_count(blob->light_count);
	
	for(size_t i = 0; i < 256; ++i)
	{
		npack->add_samples(blob->samples[i]);
	}
	
	npack->set_bounce_count(blob->bounce_count);
	
	npack->set_pic_w(blob->pic_w);
	npack->set_pic_h(blob->pic_h);
	
	npack->set_samples_per_pixel(blob->samples_per_pixel);
	
	npack->set_interleaved_feedback(blob->interleaved_feedback);
	
	
	for(size_t i = 0; i < blob->prim_count; ++i)
	{
		if (blob->primitives[i]->GetType() == SpherePrim)
		{
			raytracer::DataBlob_PrimitiveSphere* sphere_pack = npack->add_spheres();//new raytracer::DataBlob::PrimitiveSphere();
			raytracer::DataBlob_Vec3f* pos_pack = sphere_pack->mutable_position();
			pos_pack->set_x(((Sphere*)(blob->primitives[i]))->p.x);
			pos_pack->set_y(((Sphere*)(blob->primitives[i]))->p.y);
			pos_pack->set_z(((Sphere*)(blob->primitives[i]))->p.z);
			
			//sphere_pack->set_position(pos_pack);
			sphere_pack->set_size(((Sphere*)(blob->primitives[i]))->r);
			
			//npack->add_spheres(sphere_pack);
		}
	}
	
	return npack;
	
}

int renderstatus_cb(lua_State* L)
{
	lua_pushnumber(L, total_done);
	lua_pushnumber(L, total_count);
	lua_pushnumber(L, (int)render_timer.Interval());
	return 3;
}

int startrender_cb(lua_State* L)
{
	
	raytracer::DataBlob* new_pack = gen_packet_from_blob(&blob);
	
	Connection *conn = cman->new_connection(ORIGIN);
	if (!cman->connect_connection(conn, (char*)lua_tostring(L, 1), lua_tonumber(L, 2)))
	{
		lua_pushstring(L, "Could not connect!");
		return 1;
	}
	
	client::Hello* hello_pack = new client::Hello();
	hello_pack->set_address("localhost");
	hello_pack->set_port(0);
	hello_pack->set_session_id(-1);
	LiPacket* hello_lipack = new LiPacket(conn, hello_pack, C_HELLO);
	cman->send((Packet*)hello_lipack);
	
	bool ready_to_send = false;
	while (!ready_to_send)
	{
		Util::Array<LiPacket*>* in = (Pxf::Util::Array<LiPacket*>*)cman->recv_packets();
		
		for(Util::Array<LiPacket*>::iterator tpacket = in->begin(); tpacket != in->end(); ++tpacket)
		{
			(*tpacket)->get_type();
			
			if ((*tpacket)->message_type == PING)
			{
				lightning::Ping *ping = (lightning::Ping*)((*tpacket)->unpack());
				lightning::Pong *pong = new lightning::Pong();
				pong->set_ping_data(ping->ping_data());
				LiPacket *pkg = new LiPacket((*tpacket)->connection, pong, PONG);
				
				cman->send((Packet*)pkg);
				Pxf::Message("oae", "Got PING message!");
			} else if ((*tpacket)->message_type == OK)
			{
				// Send alloc request
				client::AllocateClient* alloc_reqpack = new client::AllocateClient();
				alloc_reqpack->set_amount(0); // TODO: Send real amount of tasks
				alloc_reqpack->set_batchhash("LOLWUT"); // TODO: Create a real hash of the batch data blob
				LiPacket* alloc_reqlipack = new LiPacket(conn, alloc_reqpack, C_ALLOCATE);
				cman->send((Packet*)alloc_reqlipack);
				
				Pxf::Message("oae", "Got OK message!");
				
			} else if ((*tpacket)->message_type == C_ALLOC_RESP)
			{
				Pxf::Message("oae", "Got C_ALLOC_RESP message!");
				
				// Send data!
				client::Data* data_pack = new client::Data();
				data_pack->set_batchhash("LOLWUT");
				data_pack->set_datasize(new_pack->ByteSize());
				data_pack->set_datatype(RAYTRACER);
				data_pack->set_data(new_pack->SerializeAsString());
				data_pack->set_returnaddress("127.0.0.1");
				data_pack->set_returnport(4632);
				
				LiPacket* data_lipack = new LiPacket(conn, data_pack, C_DATA);
				cman->send((Packet*)data_lipack);
				
				
				// Send tasks!
				client::Tasks* tasks_pack = new client::Tasks();
				tasks_pack->set_batchhash("LOLWUT");
				for(int y = 0; y < task_count; y++)
				{
					for(int x = 0; x < task_count; x++)
					{
						raytracer::Task* task_pack = new raytracer::Task();
						task_pack->set_x(x * task_size_w);
						task_pack->set_y(y * task_size_h);
						task_pack->set_w(task_size_w);
						task_pack->set_h(task_size_h);
						
						client::Tasks_Task* ctask_pack = tasks_pack->add_task();
						ctask_pack->set_tasksize(task_pack->ByteSize());
						ctask_pack->set_task(task_pack->SerializeAsString());
					}
				}
				LiPacket* tasks_lipack = new LiPacket(conn, tasks_pack, C_TASKS);
				cman->send((Packet*)tasks_lipack);
				
				ready_to_send = true;
				
			} else {
				Pxf::Message("oae", "Got unknown packet type!");
			}
		}
		
		in->clear();
	}
	

	lua_pushstring(L, "Sent data!");
	return 1;
}

int main(int argc, char* argv[])
{
	Pxf::RandSetSeed(time(NULL));
	Kernel* kernel = Pxf::Kernel::GetInstance();
	kernel->RegisterModule("pri", 0xFFFFFFFF, true);
	kernel->RegisterModule("img", Pxf::System::SYSTEM_TYPE_RESOURCE_LOADER, true);
	kernel->RegisterModule("mesh",Pxf::System::SYSTEM_TYPE_RESOURCE_LOADER, true);
	
	Resource::ResourceManager* res = kernel->GetResourceManager();
	Graphics::GraphicsDevice* gfx = kernel->GetGraphicsDevice();
	Input::InputDevice* inp = kernel->GetInputDevice();
	
	res->DumpResourceLoaders();

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

	Resource::Mesh::mesh_descriptor* descr;
	Resource::Mesh* box = res->Acquire<Resource::Mesh>("data/box.ctm");
	descr = box->GetData();
	Vec3f* box_vertices = (Vec3f*)descr->vertices;
	Vec3f* box_normals = (Vec3f*)descr->normals;

	Resource::Mesh* teapot = res->Acquire<Resource::Mesh>("data/teapot.ctm");
	descr = teapot->GetData();
	Vec3f* teapot_vertices = (Vec3f*)descr->vertices;
	Vec3f* teapot_normals = (Vec3f*)descr->normals;
	
	// Setup connection manager and stuff!
	cman = new ConnectionManager((Pxf::Util::Array<Packet*>*)(new Pxf::Util::Array<LiPacket*>));
	Connection *conn = cman->new_connection(ORIGIN);
	if (!cman->bind_connection(conn, "localhost", 4632))
	{
		Pxf::Message("aoe", "Bind failed!");
		exit(2);
	}
	
	
	// Generate awesome red output buffer
	//const int w = 128;
	//const int h = 128;
	//const int channels = 3;
	//const int task_count = 8;
	//int task_size_w = w / task_count;
	//int task_size_h = h / task_count;
	char pixels[w*h*channels];
	
	// job specifics
	blob.pic_w = w;
	blob.pic_h = h;
	blob.samples_per_pixel = 10; // 10 -> 10*10 = 100
	blob.bounce_count = 6; // Number of reflection bounces
	blob.interleaved_feedback = 2;
	
	// add a couple of primitives to the data blob
	material_t plane_mat_white,plane_mat_red,plane_mat_green,sphere_mat1,sphere_mat2;
	plane_mat_white.ambient = Vec3f(0.1f, 0.1f, 0.1f);
	plane_mat_white.diffuse = Vec3f(1.0f, 1.0f, 1.0f);
	plane_mat_white.reflectiveness = 0.0f;
	plane_mat_red.ambient = Vec3f(0.1f, 0.0f, 0.0f);
	plane_mat_red.diffuse = Vec3f(1.0f, 0.0f, 0.0f);
	plane_mat_red.reflectiveness = 0.0f;
	plane_mat_green.ambient = Vec3f(0.0f, 0.1f, 0.0f);
	plane_mat_green.diffuse = Vec3f(0.0f, 1.0f, 0.0f);
	plane_mat_green.reflectiveness = 0.0f;
	
	sphere_mat1.ambient = Vec3f(0.1f, 0.1f, 0.1f);
	sphere_mat1.diffuse = Vec3f(1.0f, 1.0f, 1.0f);
	sphere_mat1.reflectiveness = 1.0f;
	sphere_mat1.matteness = 0.0f;
	sphere_mat2.ambient = Vec3f(0.1f, 0.1f, 0.1f);
	sphere_mat2.diffuse = Vec3f(1.0f, 1.0f, 1.0f);
	sphere_mat2.reflectiveness = 1.0f;
	sphere_mat2.matteness = 1.0f;
	
	blob.prim_count = 0;
	blob.primitives[blob.prim_count++] = new Plane(Pxf::Math::Vec3f(0.0f, -5.0f, 0.0f), Pxf::Math::Vec3f(0.0f, 1.0f, 0.0f), &plane_mat_white); // bottom
	blob.primitives[blob.prim_count++] = new Plane(Pxf::Math::Vec3f(0.0f, 5.0f, 0.0f), Pxf::Math::Vec3f(0.0f, -1.0f, 0.0f), &plane_mat_white); // top
	blob.primitives[blob.prim_count++] = new Plane(Pxf::Math::Vec3f(-5.0f, 0.0f, 0.0f), Pxf::Math::Vec3f(1.0f, 0.0f, 0.0f), &plane_mat_red); // left
	blob.primitives[blob.prim_count++] = new Plane(Pxf::Math::Vec3f(5.0f, 0.0f, 0.0f), Pxf::Math::Vec3f(-1.0f, 0.0f, 0.0f), &plane_mat_green); // right
	blob.primitives[blob.prim_count++] = new Plane(Pxf::Math::Vec3f(0.0f, 0.0f, 10.0f), Pxf::Math::Vec3f(0.0f, 0.0f, -1.0f), &plane_mat_white); // back
	blob.primitives[blob.prim_count++] = new Sphere(Pxf::Math::Vec3f(0.0f, -3.0f, 4.0f), 1.5f, &sphere_mat1);
	blob.primitives[blob.prim_count++] = new Sphere(Pxf::Math::Vec3f(2.0f, 0.0f, 8.0f), 2.0f, &sphere_mat2);

	
	// Add 64 spheres on the floor, should slow down the render a bit. Compare with kd-tree.
	/*
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			blob.primitives[blob.prim_count++] = new Sphere(Math::Vec3f(x-3.5f,-4.f,y+2), .5f, (x+y)%2 == 0 ? &sphere_mat1 : &sphere_mat2);
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
	blob.lights[0] = new AreaLight(Pxf::Math::Vec3f(0.0f, 4.0f, 5.0f), 1.0f, 1.0f, Pxf::Math::Vec3f(0.0f, -1.0f, 0.0f), Pxf::Math::Vec3f(1.0f, 0.0f, 0.0f), 3, 3.0f, &light_mat1);
	//blob.lights[1] = new AreaLight(Pxf::Math::Vec3f(0.0f, -4.8f, 5.0f), 1.0f, 1.0f, Pxf::Math::Vec3f(0.0f, -1.0f, 0.0f), Pxf::Math::Vec3f(1.0f, 0.0f, 0.0f), 9, light_mat1);
	blob.light_count = 1;
	
	// create textures and primitive batches
	Texture *region_textures[task_count*task_count] = {0};
	Texture *unfinished_task_texture = Pxf::Kernel::GetInstance()->GetGraphicsDevice()->CreateTexture("data/unfinished.png");
	unfinished_task_texture->SetMagFilter(TEX_FILTER_NEAREST);
	unfinished_task_texture->SetMinFilter(TEX_FILTER_NEAREST);

	int ty = 0;
	int tx = 0;
	//int total_done = 0;
	total_count = task_count*task_count;
	
	PrimitiveBatch *pbatch = new PrimitiveBatch(Pxf::Kernel::GetInstance()->GetGraphicsDevice());

	// Fabric/GUI stuff
	Fabric::App* app = new Fabric::App(win, "fabric/main.lua");
	app->BindExternalFunction("renderstatus", renderstatus_cb);
	app->BindExternalFunction("startrender", startrender_cb);
	app->Boot();
	bool running = true;
	bool guihit = false;
	
	// MODEL
	Model* model_teapot = gfx->CreateModel(teapot);
	Model* model_box = gfx->CreateModel(box);

	Triangle* triangle_data = triangle_list(box);

	// CAMERA
	SimpleCamera cam;

	gfx->SetViewport(0, 0, win->GetWidth(), win->GetHeight());
	Math::Mat4 prjmat = Math::Mat4::Perspective(45.0f, win->GetWidth() / win->GetHeight(), 1.0f,10000.0f); // (-300.0f, 300.0f, 300.0f,-300.0f, 1.0f, 100000.0f);

	cam.SetProjectionView(prjmat);
	cam.Translate(0.0f,20.0f,100.0f);

	//blob.cam = &cam;

	// Raytracer client test
	//------------------------
	RaytracerClient client(kernel);
	//client.run_noblock();

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
	
	// DEPTH TEST
	gfx->SetDepthFunction(DF_LEQUAL);
	gfx->SetDepthState(true);

	//client.wait();
	//------------------------

	render_timer.Start();

	bool is_done = false;
	bool exec_rt = false;

	while(win->IsOpen() && running)
	{
		inp->Update();
		if (inp->GetLastKey() == Input::ESC)
			break;
			
		running = app->Update();
		guihit = app->GuiHit();
		
		if (inp->GetLastKey() == Input::ENTER)
		{
			if(!exec_rt) client.run_noblock();
			exec_rt = !exec_rt;
		}
		
		/* CAMERA FREE-FLY MODE */
		if(!exec_rt)
		{
			gfx->BindTexture(0,0);
			gfx->SetProjection(cam.GetProjectionView());
			gfx->SetModelView(cam.GetModelView());

			glClearColor(26.0f/255.0f,26.0f/255.0f,26.0f/255.0f,1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glColor3f(1.0f,1.0f,1.0f);
			
			if (!guihit)
				MoveCamera(&cam,inp);

			model_teapot->Draw();
		}
		else
		{
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
				if (region_textures[idx] == 0)
				{
					region_textures[idx] = gfx->CreateTextureFromData((const unsigned char*)res->pixels, task_size_w, task_size_h, channels);
					region_textures[idx]->SetMagFilter(TEX_FILTER_NEAREST);
					region_textures[idx]->SetMinFilter(TEX_FILTER_NEAREST);
				}
				else
				{
					region_textures[idx]->UpdateData((const unsigned char*)res->pixels, 0, 0, task_size_w, task_size_h);
				}

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

			if (total_done == total_count && !is_done)
			{
				//thread_executor.cancel();
				is_done = true;
				render_timer.Stop();
				char title[512];
				Format(title, "Render time: %d ms", render_timer.Interval());
				win->SetTitle(title);
			}
		}		

		// Reset view 
		glLoadIdentity();

		gfx->SetViewport(0, 0, win->GetWidth(), win->GetHeight());
		Math::Mat4 prjmat = Math::Mat4::Ortho(0, w, h, 0, -0.1f, 100.0f);
		gfx->SetProjection(&prjmat);
		
		app->Draw();
		
		inp->ClearLastKey();
		win->Swap();
	}

	client.cancel();
	client.wait();
	
	delete app;
	delete pbatch;

	delete kernel;

	return 0;
}
