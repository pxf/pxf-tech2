#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Hash.h>
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
#include <sstream>
#include <string.h>
#include "Renderer.h"
#include "Camera.h"

#include "connectionmanager.h"
#include "lightning.h"
#include "lightning.pb.h"
#include "client.pb.h"
#include "raytracer.pb.h"
//#include "KDTree.h"
#include "BVH.h"

#include "fabric/App.h"

#include <RaytracerClient.h>

using namespace Pxf;
using namespace Graphics;
using namespace Math;

int total_done = 0;
int total_count = 0; 
bool show_result = false;
Pxf::Timer render_timer;

// network connection
ConnectionManager *cman;
Connection *recv_conn = 0;

// global blob/scene data
batch_blob_t blob;
Graphics::VertexBuffer* tree_VB;
SimpleCamera cam;

struct scene {
	Resource::Mesh* mesh;
	Graphics::Model** mdl;

	int mdl_count;
	int mdl_mat[32];

} current_scene;

// recieved clients/parts info
Util::Map<Util::String, int> recieved_clients;

// task/batch specific globals
int w = 256;
int h = 256;
const int channels = 3;
int task_count = 8;
int task_size_w = w / task_count;
int task_size_h = h / task_count;
Texture **region_textures = 0;
Util::String current_hash_str;

void load_model(const char* path)
{
	if(!path) return;

	Kernel* kernel = Pxf::Kernel::GetInstance();
	Resource::Mesh* mesh = kernel->GetResourceManager()->Acquire<Resource::Mesh>(path);
	Graphics::Model* model = kernel->GetGraphicsDevice()->CreateModel(mesh);

	if(mesh && model)
	{
		if(current_scene.mesh)
			kernel->GetResourceManager()->Release<Resource::Mesh>(mesh);

		if(!current_scene.mdl) current_scene.mdl = new Graphics::Model*;
		current_scene.mesh = mesh;
		current_scene.mdl[0] = model;
		current_scene.mdl_count = 1;


		triangle_t* scene_data = triangle_list(mesh);
		//Primitive** scene_data = (Primitive**) triangle_list(mesh);

		int tri_count = mesh->GetData()->triangle_count;
		blob.primitives = scene_data;
		blob.prim_count = tri_count;

		std::string path_ext(path);
		path_ext += ".bvh";
		
		blob.tree = load_BVH(path_ext.c_str());

		if(!blob.tree) {
			blob.tree = build(scene_data,tri_count);
			write_BVH(path_ext.c_str(),blob.tree);
		} else {
			blob.tree->triangle_data = scene_data;
		}
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
	
	// pack camera!
	raytracer::DataBlob::Camera* c = npack->mutable_cam();
	raytracer::DataBlob::Vec3f* cp = c->mutable_position();
	cp->set_x(cam.GetPos().x);
	cp->set_y(cam.GetPos().y);
	cp->set_z(cam.GetPos().z);
	
	c->set_orient_x(cam.GetOrientation()->x);
	c->set_orient_y(cam.GetOrientation()->y);
	c->set_orient_z(cam.GetOrientation()->z);
	c->set_orient_w(cam.GetOrientation()->w);
	
	tree_t* tree = blob->tree;
	// pack tree
	raytracer::DataBlob::BVH* b = npack->mutable_tree();
	raytracer::DataBlob::Vec3f* minpos = b->mutable_minpos();
	minpos->set_x(tree->min.x);
	minpos->set_y(tree->min.y);
	minpos->set_z(tree->min.z);
	raytracer::DataBlob::Vec3f* maxpos = b->mutable_maxpos();
	maxpos->set_x(tree->max.x);
	maxpos->set_y(tree->max.y);
	maxpos->set_z(tree->max.z);

	b->set_num_nodes(tree->num_nodes);

	size_t nodes_data_size = sizeof(ca_node_t) * tree->num_nodes;
	b->set_nodes(Util::String((char*) tree->nodes,nodes_data_size));

	size_t index_list_data_size = sizeof(int) * tree->num_triangles;
	b->set_index_list(Util::String((char*) tree->index_list,index_list_data_size));

	// pack lights!
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

			light_pack->set_material_index(l->material_index);
		}
	}

	// pack materials

	size_t materials_size = sizeof(MaterialLibrary);
	npack->set_materials(Util::String((char*) &blob->materials,materials_size));

	//size_t triangle_size = sizeof(Triangle);
	size_t triangle_size = sizeof(triangle_t);
	npack->set_primitive_data(Util::String((char*) blob->primitives,triangle_size * blob->prim_count));



	return npack;
};

int clientsstatus_cb(lua_State* L)
{
	Util::Map<Util::String,int>::iterator it;
	
	// loop recieved_clients
	lua_newtable(L);
	for (it=recieved_clients.begin() ; it != recieved_clients.end(); it++ )
	{
		lua_pushnumber(L, (*it).second);
		lua_setfield (L, -2, (*it).first.c_str());
	}
	return 1;
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

		if(path && strcmp(path,"") != 0)
			load_model(path);

		return 0;
	}
	else
	{
		lua_pushstring(L, "Could not load model, no path passed to function");
		return 1;
	}
}

int stoprender_cb(lua_State* L)
{
	if (recv_conn)
	{
		cman->remove_connection(recv_conn);
	}
	recv_conn = 0;
	show_result = false;
	return 0;
}

int startrender_cb(lua_State* L)
{
	// lua: startrender(remote client host, remote client port,
  //                  results host, results port,
  //                  interleaved feedback,
  //                  gridsize,
  //                  imagesize)
	if (lua_gettop(L) != 7)
	{
		lua_pushstring(L, "Wrong parameter count to startrender(...).");
		lua_error(L);
		return 0;
	}
	
	// Open send connection
	Connection *conn = cman->new_connection(CLIENT);
	if (!cman->connect_connection(conn, (char*)lua_tostring(L, 1), lua_tonumber(L, 2)))
	{
		lua_pushboolean(L, false);
		lua_pushstring(L, "Could not connect to render client!");
		return 2;
	}
	
	// Open result connection
	if (recv_conn)
	{
		cman->remove_connection(recv_conn);
	}
	recv_conn = cman->new_connection(CLIENT);
	if (!cman->bind_connection(recv_conn, (char*)lua_tostring(L, 3), lua_tonumber(L, 4)))
	{
		cman->remove_connection(recv_conn);
		recv_conn = 0;
		lua_pushboolean(L, false);
		lua_pushstring(L, "Could not open result/recieve connection!");
		return 2;
	}
	
	// update/reset grid count/size
	task_count = lua_tointeger(L, 6);
	w = lua_tointeger(L, 7);
	h = lua_tointeger(L, 7);
	blob.pic_w = w;
	blob.pic_h = h;
	total_count = task_count*task_count;
	total_done = 0;
	task_size_w = w / task_count;
	task_size_h = h / task_count;
	
	// update reqion textures
	if (region_textures != 0)
		delete [] region_textures;
	region_textures = new Texture*[task_count*task_count];
	for(size_t i = 0; i < task_count*task_count; ++i)
	{
		region_textures[i] = 0;
	}
	
	// clear recv list
	recieved_clients.clear();
	
	// Start timer!
	render_timer.Start();
	
	// create hello packet
	client::Hello* hello_pack = new client::Hello();
	hello_pack->set_address((char*)lua_tostring(L, 3));
	hello_pack->set_port(0);
	hello_pack->set_session_id(-1);
	LiPacket* hello_lipack = new LiPacket(conn, hello_pack, C_HELLO);
	cman->send(conn, hello_lipack->data, hello_lipack->length);
	
	// Create datablob
	blob.interleaved_feedback = lua_tointeger(L, 5);
	raytracer::DataBlob* new_pack = gen_packet_from_blob(&blob);
	
	// Create hash of datablob
	Util::String serialized_batch = new_pack->SerializeAsString();
	unsigned long new_hash_num = RandUI32();//Hash((const char *)serialized_batch.c_str(), serialized_batch.size());
	std::stringstream current_hash;
	current_hash << new_hash_num;
	current_hash_str = current_hash.str();

	// Create datablob packets	
	client::Data* data_pack = new client::Data();
	data_pack->set_batchhash(current_hash_str);
	data_pack->set_datasize(new_pack->ByteSize());
	data_pack->set_datatype(RAYTRACER);
	data_pack->set_data(new_pack->SerializeAsString());
	data_pack->set_returnaddress((char*)lua_tostring(L, 3));//("129.16.72.27");//"127.0.0.1");
	data_pack->set_returnport(lua_tonumber(L, 4));
	
	
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
				alloc_reqpack->set_batchhash(current_hash_str);
				alloc_reqpack->set_datatype(RAYTRACER);
				LiPacket* alloc_reqlipack = new LiPacket(conn, alloc_reqpack, C_ALLOCATE);
				cman->send(conn, alloc_reqlipack->data, alloc_reqlipack->length);
				
				Pxf::Message("oae", "Got OK message!");
				
			} else if (tpacket->message_type == C_ALLOC_RESP)
			{
				Pxf::Message("oae", "Got C_ALLOC_RESP message!");
				
				// send datablob
				LiPacket* data_lipack = new LiPacket(conn, data_pack, C_DATA);
				cman->send(conn, data_lipack->data, data_lipack->length);

				// Send tasks!
				client::Tasks* tasks_pack = new client::Tasks();
				tasks_pack->set_batchhash(current_hash_str);
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
						//printf("id: %d x: %d y: %d w: %d h: %d\n", task_pack->id(), task_pack->x(), task_pack->y(), task_pack->w(), task_pack->h());

						client::Tasks::Task* ctask_pack = tasks_pack->add_task();
						ctask_pack->set_tasksize(task_pack->ByteSize());
						//char *lol = new char[task_pack->ByteSize()];
						ctask_pack->set_task(task_pack->SerializeAsString());
						//ctask_pack->PrintDebugString();
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
	
	show_result = true;

	lua_pushboolean(L, true);
	lua_pushstring(L, "Sent data!");
	return 2;
}

void generate_offline(const char* path) {
	Kernel* kernel = Pxf::Kernel::GetInstance();
	Resource::Mesh* mesh = kernel->GetResourceManager()->Acquire<Resource::Mesh>(path);

	// build triangle data from mesh data
	triangle_t* scene_data = triangle_list(mesh);
	int tri_count = mesh->GetData()->triangle_count;

	// build ext path
	std::string path_ext(path);
	path_ext += ".bvh";

	tree_t* tree = build(scene_data,tri_count);
	write_BVH(path_ext.c_str(),tree);

	delete tree;
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

	// generate tree offline 
	if(argc > 1) {
		char* path = argv[1];
		printf("generating BVH for %s\n",path);
		
		generate_offline(path);

		delete kernel;
		return 0;
	}

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

	// Setup connection manager and stuff!
	cman = new ConnectionManager();
	char* pixels = new char[w*h*channels];
	
	// job specifics
	blob.pic_w = w;
	blob.pic_h = h;
	blob.samples_per_pixel = 2; // 10 -> 10*10 = 100
	blob.bounce_count = 4; // Number of reflection bounces
	blob.interleaved_feedback = 1;	
	
	// add a couple of primitives to the data blob
	material_t plane_mat_white,plane_mat_red,plane_mat_green,plane_mat_blue,sphere_mat1,sphere_mat2;
	
	// white mat - floor
	plane_mat_white.ambient = Vec3f(0.1f, 0.1f, 0.1f);
	plane_mat_white.diffuse = Vec3f(0.8f, 0.8f, 0.8f);
	plane_mat_white.matteness = 0.0f;
	plane_mat_white.reflectiveness = 0.8f;

	plane_mat_red.ambient = Vec3f(0.1f, 0.0f, 0.0f);
	plane_mat_red.diffuse = Vec3f(1.0f, 0.0f, 0.0f);
	plane_mat_red.matteness = 0.0f;
	plane_mat_red.reflectiveness = 0.4f;

	plane_mat_green.ambient = Vec3f(0.0f, 0.1f, 0.0f);
	plane_mat_green.diffuse = Vec3f(0.0f, 1.0f, 0.0f);
	plane_mat_green.matteness = 0.0f;
	plane_mat_green.reflectiveness = 0.4f;
	
	plane_mat_blue.ambient = Vec3f(0.0f, 0.0f, 0.1f);
	plane_mat_blue.diffuse = Vec3f(0.0f, 0.0f, 1.0f);
	plane_mat_blue.matteness = 0.0f;
	plane_mat_blue.reflectiveness = 0.0f;
	
	blob.prim_count = 0;

	// generate a couple of random samples
	srand ( time(NULL) );
	for(int i = 0; i < 256; ++i)
	{
		blob.samples[i] = Pxf::RandFP32();
		//Pxf::Message("rand", "%d -> %f", i, blob.samples[i]);
	}
	
	// add a couple of lights to the data blob
	material_t light_mat1,light_mat2;
	light_mat1.diffuse = Vec3f(0.8f, 0.8f, 0.8f);
	light_mat1.ambient = Vec3f(0.1f,0.1f,0.1f);
	light_mat1.reflectiveness = 0.0f;

	light_mat2.diffuse = Vec3f(1.0f, 0.4f, 0.0f);
	light_mat2.ambient = Vec3f(0.3f,0.0f,0.1f);
	light_mat2.reflectiveness = 0.0f;

	blob.materials.Insert(light_mat1,0);
	blob.materials.Insert(plane_mat_white,1);
	blob.materials.Insert(plane_mat_red,2);
	blob.materials.Insert(plane_mat_green,3);
	blob.materials.Insert(plane_mat_blue,4);


	blob.lights[0] = new PointLight(Pxf::Math::Vec3f(0.0f, 560.0f, 15.0f), 0);//&light_mat1);
	//blob.lights[1] = new PointLight(Pxf::Math::Vec3f(15.0f, -3.0f, -15.0f), 1); //&light_mat2);
	//blob.lights[0] = new AreaLight(Pxf::Math::Vec3f(0.0f, 50.0f, 15.0f), 1.0f, 1.0f, Pxf::Math::Vec3f(0.0f, -1.0f, -0.5f), Pxf::Math::Vec3f(1.0f, 0.0f, 0.0f), 3, 3.0f, &light_mat1);
	//blob.lights[1] = new AreaLight(Pxf::Math::Vec3f(0.0f, 4.8f, 5.0f), 1.0f, 1.0f, Pxf::Math::Vec3f(0.0f, -1.0f, 0.0f), Pxf::Math::Vec3f(1.0f, 0.0f, 0.0f), 9, light_mat1);
	blob.light_count = 1;
	
	// create textures and primitive batches
	//Texture *region_textures[task_count*task_count] = {0};
	Texture *unfinished_task_texture = Pxf::Kernel::GetInstance()->GetGraphicsDevice()->CreateTexture("data/unfinished.png");
	unfinished_task_texture->SetMagFilter(TEX_FILTER_LINEAR);
	unfinished_task_texture->SetMinFilter(TEX_FILTER_LINEAR);

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
	app->BindExternalFunction("stoprender", stoprender_cb);
	app->BindExternalFunction("clientsstatus", clientsstatus_cb);
	app->Boot();
	bool running = true;
	bool guihit = false;

	gfx->SetViewport(0, 0, win->GetWidth(), win->GetHeight());
	Math::Mat4 prjmat = Math::Mat4::Perspective(80.0f, win->GetWidth() / win->GetHeight(), 1.0f,10000.0f); // (-300.0f, 300.0f, 300.0f,-300.0f, 1.0f, 100000.0f);

	cam.SetProjectionView(prjmat);
	//cam.Translate(0.0f,20.0f,100.0f);
	cam.Translate(0.0f,0.0f,20.0f);
	blob.cam = &cam;

	Pxf::Resource::Mesh* meshlist[2];
	//meshlist[0] = res->Acquire<Resource::Mesh>("data/scene1/floor0.ctm");
	meshlist[0] = res->Acquire<Resource::Mesh>("data/bentley.ctm");
	//meshlist[1] = res->Acquire<Resource::Mesh>("data/bentley.ctm");

	int tri_count = 0;
	for(int i=0; i < 1; i++) {
		tri_count += meshlist[i]->GetData()->triangle_count;
	}

	int matlist[] = {4};
	triangle_t* tlist = merge_meshlist(meshlist,matlist,1);
	
	//tree_t* tree = load_BVH("data/scene3/tree");
	
	tree_t* tree = build(tlist,tri_count);
	write_BVH("data/scene3/tree",tree);


	current_scene.mdl_mat[0] = matlist[0];
	//current_scene.mdl_mat[1] = matlist[1];
	//current_scene.mdl_mat[2] = matlist[2];

	Pxf::Graphics::Model* mdllist[1];
	mdllist[0] = gfx->CreateModel(meshlist[0]);
	//mdllist[1] = gfx->CreateModel(meshlist[1]);
	//mdllist[2] = gfx->CreateModel(meshlist[2]);

	current_scene.mdl = mdllist;
	current_scene.mdl_count = 1;

	blob.tree = tree;
	blob.primitives = tlist;
	blob.prim_count = tri_count;


/*
	Pxf::Resource::Mesh* meshlist[3];
	meshlist[0] = res->Acquire<Resource::Mesh>("data/scene0/sphere0.ctm");
	meshlist[1] = res->Acquire<Resource::Mesh>("data/scene0/sphere1.ctm");
	meshlist[2] = res->Acquire<Resource::Mesh>("data/scene0/floor.ctm");

	int tri_count = 0;
	for(int i=0; i < 3; i++) {
		tri_count += meshlist[i]->GetData()->triangle_count;
	}

	int matlist[] = {2,3,1};
	triangle_t* tlist = merge_meshlist(meshlist,matlist,3);

	tree_t* tree = load_BVH("data/scene0/tree0");

	//tree_t* tree = build(tlist,tri_count);
	//write_BVH("data/scene0/tree0",tree);


	current_scene.mdl_mat[0] = 2;
	current_scene.mdl_mat[1] = 3;
	current_scene.mdl_mat[2] = 1;

	Pxf::Graphics::Model* mdllist[3];
	mdllist[0] = gfx->CreateModel(meshlist[0]);
	mdllist[1] = gfx->CreateModel(meshlist[1]);
	mdllist[2] = gfx->CreateModel(meshlist[2]);

	current_scene.mdl = mdllist;
	current_scene.mdl_count = 3;

	blob.tree = tree;
	blob.primitives = tlist;
	blob.prim_count = tri_count;
	*/
	
	// DEPTH TEST
	gfx->SetDepthFunction(DF_LEQUAL);
	gfx->SetDepthState(true);

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

				//Pxf::Message("aoe", "Got packet on SOME connection!");
				tpacket->get_type();
				if (tpacket->message_type == C_RESULT)
				{
					// Got result packet from a client
					client::Result *res_packet = (client::Result*)(tpacket->unpack());
					raytracer::Result *res_raytrace_packet = new raytracer::Result();
					
					// Unravel from client::Result to raytracer::Result (which includes task id and result data)
					res_raytrace_packet->ParseFromString(res_packet->result());
					
					Pxf::Message("aoe", "Got result packet for batch: %s, result id: %d.", res_packet->batchhash().c_str(), res_raytrace_packet->id());
					
					// See if we want this packet (might be an old hash/batch)
					if (current_hash_str != res_packet->batchhash())
					{
						Pxf::Message("aoe", "Got old/wrong result (%s != %s)", current_hash_str.c_str(), res_packet->batchhash().c_str());
						continue;
					}
					
					// update recieved clients list
					if (recieved_clients.find(tpacket->connection->target_address) == recieved_clients.end())
					{
						recieved_clients.insert(pair<Util::String,int>(tpacket->connection->target_address,1));
					} else {
						recieved_clients[tpacket->connection->target_address] += 1;
					}
					
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
		else
		{
			gfx->BindTexture(0,0);
			gfx->SetProjection(cam.GetProjectionView());
			gfx->SetModelView(cam.GetModelView());

			glClearColor(26.0f/255.0f,26.0f/255.0f,26.0f/255.0f,1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glColor3f(1.0f,1.0f,1.0f);
			
			if (!guihit)
				MoveCamera(&cam,inp);

			for(size_t i=0; i < blob.light_count; i++)
				draw_light((BaseLight*) blob.lights[i]);
			
			if(current_scene.mdl) {
				for(int i = 0; i < current_scene.mdl_count; i++) {
					Vec3f c = blob.materials.GetMaterial(current_scene.mdl_mat[i]).diffuse;
					glColor3f(c.r,c.g,c.b);
					gfx->DrawBuffer(current_scene.mdl[i]->GetVertexBuffer(),0);
				}
			}

			glColor3f(1.0f,0.0f,0.0f);
			//gfx->DrawBuffer(blob.tree->debug_buffer,0);

			/*
			if(p_res)
			{
				glColor3f(0.0f,0.0f,1.0f);
				draw_triangle(*p_res);
				//p_res->Draw();
			} */
		} 

		glLoadIdentity();
		// Setup view!!!!!!!!
		gfx->SetViewport(0, 0, win->GetWidth(), win->GetHeight());
		prjmat = Math::Mat4::Ortho(0, w, h, 0, -0.1f, 100.0f);
		gfx->SetProjection(&prjmat);

		if(show_result)
		{
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

			
		}
		
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
