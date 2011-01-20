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
#include "KDTree.h"

#include "fabric/App.h"

#include <RaytracerClient.h>

using namespace Pxf;
using namespace Graphics;
using namespace Math;

int total_done = 0;
int total_count = 0;
Pxf::Timer render_timer;

// network connection
ConnectionManager *cman;
Connection *recv_conn = 0;

// global blob/scene data
batch_blob_t blob;
Graphics::VertexBuffer* tree_VB;

struct scene {
	Resource::Mesh* mesh;
	Graphics::Model* mdl;
} current_scene;


// task/batch specific globals
const int w = 128;
const int h = 128;
const int channels = 3;
const int task_count = 8;
int task_size_w = w / task_count;
int task_size_h = h / task_count;

void load_model(const char* path)
{
	Kernel* kernel = Pxf::Kernel::GetInstance();
	Resource::Mesh* mesh = kernel->GetResourceManager()->Acquire<Resource::Mesh>(path);
	Graphics::Model* model = kernel->GetGraphicsDevice()->CreateModel(mesh);

	if(mesh && model)
	{
		if(current_scene.mesh)
			kernel->GetResourceManager()->Release<Resource::Mesh>(mesh);

		current_scene.mesh = mesh;
		current_scene.mdl = model;

		Primitive** scene_data = (Primitive**) triangle_list(mesh);
		int tri_count = mesh->GetData()->triangle_count;

		// TODO: update/fix this when we are using triangles again!
		/*blob.tree = new KDTree(3);
		blob.tree->Build(scene_data,tri_count);
		blob.primitives = scene_data;
		blob.prim_count = tri_count;*/

		if(tree_VB)
			kernel->GetGraphicsDevice()->DestroyVertexBuffer(tree_VB);

		tree_VB = kernel->GetGraphicsDevice()->CreateVertexBuffer(Graphics::VB_LOCATION_GPU,Graphics::VB_USAGE_STATIC_DRAW);
		CreateVBFromTree(blob.tree,tree_VB);
	}
	else
		Pxf::Message("Main","Unable to load model");

}

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
	
	
	for(size_t i = 0; i < blob->light_count; i++)
	{
		if(blob->lights[i] && blob->lights[i]->GetType() == PointLightPrim)
		{
			PointLight* l = (PointLight*) blob->lights[i];
			raytracer::DataBlob::PointLight* light_pack = npack->add_lights();
			raytracer::DataBlob::Vec3f* p = light_pack->mutable_position();
			p->set_x(l->p.x);
			p->set_y(l->p.y);
			p->set_z(l->p.z);
		}
	}
	
	
	
	for(size_t i = 0; i < blob->prim_count; i++)
	{
	/*
		Primitive* p = blob->primitives[i];
		if (p && (p->GetType() == TrianglePrim))
		{
			Triangle* t = (Triangle*) p;

			raytracer::DataBlob_PrimitiveTriangle* triangle_pack = npack->add_triangles();
			
			// VERTEX 0
			raytracer::DataBlob_Vertex* v0 = triangle_pack->mutable_v0();
			raytracer::DataBlob_Vec3f* v0_p = v0->mutable_p();
			v0_p->set_x(t->vertices[0]->v.x);
			v0_p->set_y(t->vertices[0]->v.y);
			v0_p->set_z(t->vertices[0]->v.z);

			raytracer::DataBlob_Vec3f* v0_n = v0->mutable_n();
			v0_n->set_x(t->vertices[0]->n.x);
			v0_n->set_y(t->vertices[0]->n.y);
			v0_n->set_z(t->vertices[0]->n.z);

			// VERTEX 1
			raytracer::DataBlob_Vertex* v1 = triangle_pack->mutable_v1();
			raytracer::DataBlob_Vec3f* v1_p = v1->mutable_p();
			v1_p->set_x(t->vertices[1]->v.x);
			v1_p->set_y(t->vertices[1]->v.y);
			v1_p->set_z(t->vertices[1]->v.z);

			raytracer::DataBlob_Vec3f* v1_n = v1->mutable_n();
			v1_n->set_x(t->vertices[1]->n.x);
			v1_n->set_y(t->vertices[1]->n.y);
			v1_n->set_z(t->vertices[1]->n.z);

			// VERTEX 2
			raytracer::DataBlob_Vertex* v2 = triangle_pack->mutable_v2();
			raytracer::DataBlob_Vec3f* v2_p = v2->mutable_p();
			v2_p->set_x(t->vertices[2]->v.x);
			v2_p->set_y(t->vertices[2]->v.y);
			v2_p->set_z(t->vertices[2]->v.z);

			raytracer::DataBlob_Vec3f* v2_n = v2->mutable_n();
			v2_n->set_x(t->vertices[2]->n.x);
			v2_n->set_y(t->vertices[2]->n.y);
			v2_n->set_z(t->vertices[2]->n.z);
		}
		else */
		if (blob->primitives[i]->GetType() == SpherePrim)
		{
			raytracer::DataBlob_PrimitiveSphere* sphere_pack = npack->add_primitives();
			raytracer::DataBlob_Vec3f* pos_pack = sphere_pack->mutable_position();
			pos_pack->set_x(((Sphere*)(blob->primitives[i]))->p.x);
			pos_pack->set_y(((Sphere*)(blob->primitives[i]))->p.y);
			pos_pack->set_z(((Sphere*)(blob->primitives[i]))->p.z);
			
			sphere_pack->set_size(((Sphere*)(blob->primitives[i]))->r);
			
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

int loadmodel_cb(lua_State* L)
{
	if(lua_gettop(L) == 1)
	{
		const char* path = lua_tostring(L,1);
		load_model(path);

		return 0;
	}
	else
	{
		lua_pushstring(L, "Could not load model, no path passed to function");
		return 1;
	}
}

int startrender_cb(lua_State* L)
{
	
	// Open result connection
	recv_conn = cman->new_connection(ORIGIN);
	if (!cman->bind_connection(recv_conn, (char*)lua_tostring(L, 3), lua_tonumber(L, 4)))
	{
		cman->remove_connection(recv_conn);
		recv_conn = 0;
		lua_pushstring(L, "Could not open result/recieve port!");
		return 1;
	}
	
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
	cman->send(conn, hello_lipack->data, hello_lipack->length);
	
	bool ready_to_send = false;
	while (!ready_to_send)
	{
		Util::Array<Packet*>* in = cman->recv_packets();
		
		for(Util::Array<Packet*>::iterator i_tpacket = in->begin(); i_tpacket != in->end(); ++i_tpacket)
		{
            LiPacket* tpacket = new LiPacket((*i_tpacket));

			tpacket->get_type();
			
			if (tpacket->message_type == PING)
			{
				lightning::Ping *ping = (lightning::Ping*)(tpacket->unpack());
				lightning::Pong *pong = new lightning::Pong();
				pong->set_ping_data(ping->ping_data());
				LiPacket *pkg = new LiPacket(tpacket->connection, pong, PONG);
				
				cman->send(conn, pkg->data, pkg->length);
				Pxf::Message("oae", "Got PING message!");
			} else if (tpacket->message_type == OK)
			{
				// Send alloc request
				client::AllocateClient* alloc_reqpack = new client::AllocateClient();
				alloc_reqpack->set_amount(0); // TODO: Send real amount of tasks
				alloc_reqpack->set_batchhash("LOLWUT"); // TODO: Create a real hash of the batch data blob
				LiPacket* alloc_reqlipack = new LiPacket(conn, alloc_reqpack, C_ALLOCATE);
				cman->send(conn, alloc_reqlipack->data, alloc_reqlipack->length);
				
				Pxf::Message("oae", "Got OK message!");
				
			} else if (tpacket->message_type == C_ALLOC_RESP)
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
				cman->send(conn, data_lipack->data, data_lipack->length);
				
				
				// Send tasks!
				client::Tasks* tasks_pack = new client::Tasks();
				tasks_pack->set_batchhash("LOLWUT");
				for(int y = 0; y < task_count; y++)
				{
					for(int x = 0; x < task_count; x++)
					{
						raytracer::Task* task_pack = new raytracer::Task();
						task_pack->set_id(y*task_count+x);
						task_pack->set_x(x * task_size_w);
						task_pack->set_y(y * task_size_h);
						task_pack->set_w(task_size_w);
						task_pack->set_h(task_size_h);
						printf("id: %d x: %d y: %d w: %d h: %d\n", task_pack->id(), task_pack->x(), task_pack->y(), task_pack->w(), task_pack->h());
						
						client::Tasks::Task* ctask_pack = tasks_pack->add_task();
						ctask_pack->set_tasksize(task_pack->ByteSize());
						//char *lol = new char[task_pack->ByteSize()];
						ctask_pack->set_task(task_pack->SerializeAsString());
						ctask_pack->PrintDebugString();
						//task_pack->SerializeToArray(lol, task_pack->ByteSize());
						//task_pack->SerializeToString(lol);//, task_pack->ByteSize());
						//ctask_pack->set_task(lol);
						
						//delete [] lol;
					}
				}
				LiPacket* tasks_lipack = new LiPacket(conn, tasks_pack, C_TASKS);
				cman->send(conn, tasks_lipack->data, tasks_lipack->length);
				
				cman->remove_connection(conn);
				
				ready_to_send = true;
				
			} else {
				Pxf::Message("oae", "Got unknown packet type %d!", tpacket->message_type);
			}
            delete tpacket;
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

	if (!win)
	{
		kernel->Log(0, "No window. Exiting.");
		return 1;
	}


	Resource::Mesh::mesh_descriptor* descr;
	Resource::Mesh* box = res->Acquire<Resource::Mesh>("data/box.ctm");
	//Resource::Mesh* box2 = res->Acquire<Resource::Mesh>("data/box_2.ctm");
	Resource::Mesh* sphere = res->Acquire<Resource::Mesh>("data/sphere.ctm");
	Resource::Mesh* teapot = res->Acquire<Resource::Mesh>("data/teapot.ctm");

	// Setup connection manager and stuff!
	cman = new ConnectionManager();
	char pixels[w*h*channels];
	
	// job specifics
	blob.pic_w = w;
	blob.pic_h = h;
	blob.samples_per_pixel = 4; // 10 -> 10*10 = 100
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

	/*
	blob.primitives[blob.prim_count++] = new Plane(Pxf::Math::Vec3f(0.0f, -5.0f, 0.0f), Pxf::Math::Vec3f(0.0f, 1.0f, 0.0f), &plane_mat_white); // bottom
	blob.primitives[blob.prim_count++] = new Plane(Pxf::Math::Vec3f(0.0f, 5.0f, 0.0f), Pxf::Math::Vec3f(0.0f, -1.0f, 0.0f), &plane_mat_white); // top
	blob.primitives[blob.prim_count++] = new Plane(Pxf::Math::Vec3f(-5.0f, 0.0f, 0.0f), Pxf::Math::Vec3f(1.0f, 0.0f, 0.0f), &plane_mat_red); // left
	blob.primitives[blob.prim_count++] = new Plane(Pxf::Math::Vec3f(5.0f, 0.0f, 0.0f), Pxf::Math::Vec3f(-1.0f, 0.0f, 0.0f), &plane_mat_green); // right
	blob.primitives[blob.prim_count++] = new Plane(Pxf::Math::Vec3f(0.0f, 0.0f, 10.0f), Pxf::Math::Vec3f(0.0f, 0.0f, -1.0f), &plane_mat_white); // back
	*/
	blob.primitives[blob.prim_count++] = new Sphere(Pxf::Math::Vec3f(0.0f, -3.0f, 4.0f), 1.5f, &sphere_mat1);
	blob.primitives[blob.prim_count++] = new Sphere(Pxf::Math::Vec3f(2.0f, 0.0f, 8.0f), 2.0f, &sphere_mat2);

	//blob.prim_count = 2;
	
	// generate a couple of random samples
	srand ( time(NULL) );
	for(int i = 0; i < 256; ++i)
	{
		blob.samples[i] = Pxf::RandFP32();
		//Pxf::Message("rand", "%d -> %f", i, blob.samples[i]);
	}
	
	// add a couple of lights to the data blob
	material_t light_mat1,light_mat2;
	light_mat1.diffuse = Vec3f(0.0f, 0.0f, 1.0f);
	light_mat2.diffuse = Vec3f(1.0f, 0.0f, 0.0f);
	blob.lights[0] = new PointLight(Pxf::Math::Vec3f(0.0f, 60.0f, 15.0f), &light_mat1);
	blob.lights[1] = new PointLight(Pxf::Math::Vec3f(15.0f, -20.0f, -15.0f), &light_mat2);
	//blob.lights[0] = new AreaLight(Pxf::Math::Vec3f(0.0f, 50.0f, 15.0f), 1.0f, 1.0f, Pxf::Math::Vec3f(0.0f, -1.0f, -0.5f), Pxf::Math::Vec3f(1.0f, 0.0f, 0.0f), 3, 3.0f, &light_mat1);
	//blob.lights[1] = new AreaLight(Pxf::Math::Vec3f(0.0f, 4.8f, 5.0f), 1.0f, 1.0f, Pxf::Math::Vec3f(0.0f, -1.0f, 0.0f), Pxf::Math::Vec3f(1.0f, 0.0f, 0.0f), 9, light_mat1);
	blob.light_count = 2;
	
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
	app->BindExternalFunction("loadmodel", loadmodel_cb);
	app->BindExternalFunction("startrender", startrender_cb);
	app->Boot();
	bool running = true;
	bool guihit = false;
	
	// MODELS
	Model* model_teapot = gfx->CreateModel(teapot);
	Model* model_box = gfx->CreateModel(box);
	//Model* model_box2 = gfx->CreateModel(box2);
	Model* model_sphere = gfx->CreateModel(sphere);

	current_scene.mesh = teapot;
	current_scene.mdl = model_teapot;

	// CAMERA
	SimpleCamera cam;

	gfx->SetViewport(0, 0, win->GetWidth(), win->GetHeight());
	Math::Mat4 prjmat = Math::Mat4::Perspective(80.0f, win->GetWidth() / win->GetHeight(), 1.0f,10000.0f); // (-300.0f, 300.0f, 300.0f,-300.0f, 1.0f, 100000.0f);

	cam.SetProjectionView(prjmat);
	cam.Translate(0.0f,20.0f,100.0f);
	blob.cam = &cam;


	// load a model!
	//load_model("data/box_2.ctm");

	// Raytracer client test
	//------------------------
	//RaytracerClient client(kernel);
	//client.run_noblock();

	// add a bunch of tasks
	/*for(int y = 0; y < task_count; y++)
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
	}*/
	
	// DEPTH TEST
	gfx->SetDepthFunction(DF_LEQUAL);
	gfx->SetDepthState(true);

	//client.wait();
	//------------------------

	render_timer.Start();

	bool is_done = false;
	bool exec_rt = false;

	float t = 0.0f;

	while(win->IsOpen() && running)
	{
		inp->Update();
		if (inp->GetLastKey() == Input::ESC)
			break;
			
		running = app->Update();
		guihit = app->GuiHit();
		
		// Recieve results via the network
		if (recv_conn != 0)
		{
			Util::Array<Packet*>* in = cman->recv_packets();
			for(Util::Array<Packet*>::iterator i_tpacket = in->begin(); i_tpacket != in->end(); ++i_tpacket)
			{
				LiPacket* tpacket = new LiPacket((*i_tpacket));

				Pxf::Message("aoe", "Got packet on SOME connection!");
				tpacket->get_type();
				if (tpacket->message_type == C_RESULT)
				{
					// Got result packet from a client
					client::Result *res_packet = (client::Result*)(tpacket->unpack());
					raytracer::Result *res_raytrace_packet = new raytracer::Result();
					
					// Unravel from client::Result to raytraycer::Result (which includes task id and result data)
					res_raytrace_packet->ParseFromString(res_packet->result());
					
					Pxf::Message("aoe", "Got result packet for batch: %s, result id: %d.", res_packet->batchhash().c_str(), res_raytrace_packet->id());
					
					
					// Create texture from incomming data
					int x = res_raytrace_packet->x() / task_size_w;
					int y = res_raytrace_packet->y() / task_size_h;

					unsigned int idx = res_raytrace_packet->id();
					Pxf::Graphics::GraphicsDevice* gfx = Pxf::Kernel::GetInstance()->GetGraphicsDevice();
					if (region_textures[idx] == 0)
					{
						region_textures[idx] = gfx->CreateTextureFromData((const unsigned char*)res_raytrace_packet->data().c_str(), task_size_w, task_size_h, channels);
						region_textures[idx]->SetMagFilter(TEX_FILTER_NEAREST);
						region_textures[idx]->SetMinFilter(TEX_FILTER_NEAREST);
					}
					else
					{
						region_textures[idx]->UpdateData((const unsigned char*)res_raytrace_packet->data().c_str(), 0, 0, task_size_w, task_size_h);
					}
					
					if (res_raytrace_packet->final())
						total_done += 1;

				}
                delete tpacket;
			}

			in->clear();
		}
		
		/*
		if (inp->GetLastKey() == Input::ENTER)
		{
			if(!exec_rt) client.run_noblock();
			exec_rt = !exec_rt;
		}
		
		// CAMERA FREE-FLY MODE
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

			current_scene.mdl->Draw();

			for(size_t i=0; i < blob.light_count; i++)
				draw_light((BaseLight*) blob.lights[i]);

			gfx->DrawBuffer(tree_VB,0);

			// INIT DEBUG RAY
			t += 0.01f;

			ray_t debug_ray;
			debug_ray.o = Math::Vec3f(5.0f,25.0f + sin(t*0.25f)*10.0f,50.0f);
			debug_ray.d = Math::Vec3f(0.0f,-0.2f,-1.0f);
			Normalize(debug_ray.d);

			Vec3f p0 = debug_ray.o;
			Vec3f p1 = debug_ray.o + debug_ray.d * 10000.0f;

			// DRAW DEBUG RAY
			glColor3f(0.0f,1.0f,0.0f);
			glBegin(GL_LINES);
				glVertex3f(p0.x,p0.y,p0.z);
				glVertex3f(p1.x,p1.y,p1.z);
			glEnd();

			intersection_response_t resp;
			Primitive* p_res = RayTreeIntersect(*blob.tree,debug_ray,10000.0f,resp);

			if(p_res)
			{
				p_res->Draw();
			}

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
			*/
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
				sprintf(title, "Render time: %d ms", render_timer.Interval());
				win->SetTitle(title);
			}
		//}		

		// Reset view 
		glLoadIdentity();

		gfx->SetViewport(0, 0, win->GetWidth(), win->GetHeight());
		Math::Mat4 prjmat = Math::Mat4::Ortho(0, w, h, 0, -0.1f, 100.0f);
		gfx->SetProjection(&prjmat);
		
		app->Draw();
		
		inp->ClearLastKey();
		win->Swap();
	}

	//client.cancel();
	//client.wait();
	
	delete app;
	delete pbatch;

	delete kernel;

	return 0;
}
