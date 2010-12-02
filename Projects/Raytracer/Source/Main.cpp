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
#include <Pxf/Graphics/VertexBuffer.h>
#include <Pxf/Graphics/WindowSpecifications.h>
#include <Pxf/Graphics/Texture.h>
#include <Pxf/Graphics/PrimitiveBatch.h>
#include <Pxf/Modules/pri/OpenGL.h>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include "Renderer.h"
#include "Camera.h"

#include <zthread/PoolExecutor.h>

using namespace Pxf;
using namespace Graphics;
using namespace Math;


class RegionTask : public ZThread::Runnable
{
private:
	Pxf::Kernel* m_Kernel;
	task_detail_t* m_Task;
	batch_blob_t* m_Blob;
	render_result_t* m_Result;
public:
	RegionTask(Pxf::Kernel* _Kernel, int _ThreadID, task_detail_t* _pTask, batch_blob_t* _pBlob, render_result_t* _pResult)
		: m_Kernel(_Kernel)
		, m_Task(_pTask)
		, m_Blob(_pBlob)
		, m_Result(_pResult)
	{}

	void run()
	{
		m_Kernel->Log(0, "doing id: %d", m_Task->task_id);
		render_task(m_Task, m_Blob, m_Result);
	}
};

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
	const int w = 512;
	const int h = 512;
	const int channels = 3;
	const int task_count = 8;
	int task_size_w = w / task_count;
	int task_size_h = h / task_count;
	char pixels[w*h*channels];
	
	// job specifics
	batch_blob_t blob;
	blob.pic_w = w;
	blob.pic_h = h;
	blob.samples_per_pixel = 2;
	
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
	sphere_mat1.reflectiveness = 0.9f;

	material_t white_mat;
	white_mat.ambient = Vec3f(0.1f, 0.1f, 0.1f);
	white_mat.diffuse = Vec3f(1.0f, 1.0f, 1.0f);
	white_mat.reflectiveness = 0.3f;

	MaterialLibrary mLib;
	mLib.Insert(white_mat,0);

	blob.prim_count = 0;

#define MAX_W 8
#define MAX_H 8
#define MAX_D 8

	for(int x = 0; x < MAX_W; x++)
	{
		for(int y = 0; y<MAX_H; y++)
		{
			for(int z = 0; z < MAX_D; z++)
			{
				blob.pData[blob.prim_count++] = new Prim(Vec3f(x*2.0f,y*2.0f,z*2.0f),Vec3f(x*2.0f+0.5f,y*2.0f+0.5f,z*2.0f),Vec3f(x*2.0f+1.0f,y*2.0f,z*2.0f+0.5f)); //new Prim(Vec3f(x,y,z+0.5f),Vec3f(x+2.0f,y,z-0.1f),Vec3f(x+1.0f,y+2.0f,z));
			}
		}
	}
	blob.tree = new KDTree(3);

	blob.tree->Build(blob.pData,blob.prim_count);
	
	// generate a couple of random samples
	srand ( time(NULL) );
	for(int i = 0; i < 256; ++i)
	{
		blob.samples[i] = Pxf::RandFP32();
	}
	
	// add a couple of lights to the data blob
	material_t light_mat1,light_mat2;
	light_mat1.diffuse = Vec3f(1.0f, 1.0f, 1.0f);
	light_mat2.diffuse = Vec3f(1.0f, 1.0f, 1.0f);
	blob.lights[0] = new AreaLight(Pxf::Math::Vec3f(0.0f, 4.8f, 5.0f), 1.0f, 1.0f, Pxf::Math::Vec3f(0.0f, -1.0f, 0.0f), Pxf::Math::Vec3f(1.0f, 0.0f, 0.0f), 5, light_mat1);
	blob.lights[1] = new AreaLight(Pxf::Math::Vec3f(0.0f, -4.8f, 5.0f), 1.0f, 1.0f, Pxf::Math::Vec3f(0.0f, -1.0f, 0.0f), Pxf::Math::Vec3f(1.0f, 0.0f, 0.0f), 5, light_mat1);
	blob.light_count = 2;
	
	blob.lData[0] = Prim(Pxf::Math::Vec3f(0.0f, 4.8f, 5.0f), 1.0f, 1.0f, Pxf::Math::Vec3f(0.0f, -1.0f, 0.0f), Pxf::Math::Vec3f(1.0f, 0.0f, 0.0f), 5);
	blob.lData[1] = Prim(Pxf::Math::Vec3f(0.0f, -4.8f, 5.0f), 1.0f, 1.0f, Pxf::Math::Vec3f(0.0f, -1.0f, 0.0f), Pxf::Math::Vec3f(1.0f, 0.0f, 0.0f), 5);

	// create textures and primitive batches
	Texture *region_textures[task_count*task_count] = {0};
	Texture *unfinished_task_texture = Pxf::Kernel::GetInstance()->GetGraphicsDevice()->CreateTexture("unfinished.png");
	unfinished_task_texture->SetMagFilter(TEX_FILTER_NEAREST);
	unfinished_task_texture->SetMinFilter(TEX_FILTER_NEAREST);
	
	int ty = 0;
	int tx = 0;
	int total_done = 0;
	
	PrimitiveBatch *pbatch = new PrimitiveBatch(Pxf::Kernel::GetInstance()->GetGraphicsDevice());

	static const int THREAD_COUNT = 2;
	ZThread::PoolExecutor thread_executor(THREAD_COUNT);

	Pxf::Timer render_timer;

	Pxf::Graphics::VertexBuffer* vb = gfx->CreateVertexBuffer(VB_LOCATION_GPU,VB_USAGE_STATIC_DRAW);
	
	vb->CreateNewBuffer(blob.prim_count * 3,sizeof(Pxf::Math::Vec3f));
	vb->SetData(VB_VERTEX_DATA,0,3);
	vb->SetPrimitive(VB_PRIMITIVE_TRIANGLES);

	Vec3f* vBuffer;
	vBuffer = (Vec3f*)vb->MapData(VB_ACCESS_WRITE_ONLY);

	for(size_t i=0; i < blob.prim_count; i++)
	{
		vBuffer[i*3] = blob.pData[i]->v[0];
		vBuffer[i*3+1] = blob.pData[i]->v[1];
		vBuffer[i*3+2] = blob.pData[i]->v[2];
	}

	vb->UnmapData();

	Pxf::Graphics::VertexBuffer* tree_vb = gfx->CreateVertexBuffer(VB_LOCATION_GPU,VB_USAGE_STATIC_DRAW);
	CreateVBFromTree(blob.tree,tree_vb);

	bool is_done = false;

	SimpleCamera cam;

	gfx->SetViewport(0, 0, win->GetWidth(), win->GetHeight());
	Math::Mat4 prjmat = Math::Mat4::Perspective(45.0f, win->GetWidth() / win->GetHeight(), 1.0f,10000.0f); // (-300.0f, 300.0f, 300.0f,-300.0f, 1.0f, 100000.0f);

	cam.SetProjectionView(prjmat);
	cam.Translate(0.0f,0.0f,10.0f);

	bool exec_rt = false;

	blob.cam = &cam;

	float t=0.0f;
	glDepthFunc(GL_LEQUAL);

	while(win->IsOpen())
	{
		inp->Update();
		if (inp->GetLastKey() == Input::ESC)
			break;

		if(inp->GetLastKey() == Input::ENTER)
		{
			exec_rt = !exec_rt;

			if(exec_rt)
			{
				// reset rendering
				render_timer.Start();
				is_done = false;
			}
		}

		if(!exec_rt)
		{
			gfx->BindTexture(0,0);
			gfx->SetProjection(cam.GetProjectionView());
			gfx->SetModelView(cam.GetModelView());

			glClearColor(26.0f/255.0f,26.0f/255.0f,26.0f/255.0f,1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glColor3f(1.0f,1.0f,1.0f);

			glEnable(GL_DEPTH_TEST);
		
			MoveCamera(&cam,inp);

			gfx->DrawBuffer(vb,0);
			gfx->DrawBuffer(tree_vb,0);

			// debug ray
			ray_t ray;

			ray_t ray2;
			//ray.o = Vec3f(sin(t)*5.0f+2.5f,0.5f,cos(t)*1.5f-10.0f);
			//ray.d = Vec3f(2.0f + sin(t)*2.0f,0.3f,1.0f) - ray.o; //Vec3f(0.0f,0.0f,sin(t)*2.0f);

			ray.o = Vec3f(0.5f,2.25f,-10.0f);
			ray.d = Vec3f(sin(t*0.1)*0.2f,sin(t*0.1),1.0f);

			ray2.o = Vec3f(2.5f,0.25f,-10.0f);
			ray2.d = Vec3f(sin(t*0.1)*0.2f,sin(t*0.1),1.0f);

			//Vec3f c = Vec3f(0.0f,0.0f,0.0f) - ray.o;

			Normalize(ray.d);
			Normalize(ray2.d);

			Vec3f p2 = ray.o + ray.d * 100.0f;
			Vec3f p3 = ray2.o + ray2.d * 100.0f;

			t+=0.001f;

			// draw  debug ray2
			glColor3f(0.0f,1.0f,0.0f);
			glBegin(GL_LINES);

			glVertex3f(ray.o.x,ray.o.y,ray.o.z);
			glVertex3f(p2.x,p2.y,p2.z);
			
			glVertex3f(ray2.o.x,ray2.o.y,ray2.o.z);
			glVertex3f(p3.x,p3.y,p3.z);

			glEnd();

			intersection_response_t debug_resp;
			Prim* p = RayTreeIntersect((*blob.tree),ray,100.0f,debug_resp);
			Prim* b = RayTreeIntersect((*blob.tree),ray2,100.0f,debug_resp);

			if(p)
			{
				glBegin(GL_TRIANGLES);
				glColor3f(1.0f,0.0f,0.0f);
				glVertex3f(p->v[0].x,p->v[0].y,p->v[0].z);
				glVertex3f(p->v[1].x,p->v[1].y,p->v[1].z);
				glVertex3f(p->v[2].x,p->v[2].y,p->v[2].z);
				glEnd();
			}

			if(b)
			{
				glBegin(GL_TRIANGLES);
				glColor3f(1.0f,0.0f,0.0f);
				glVertex3f(b->v[0].x,b->v[0].y,b->v[0].z);
				glVertex3f(b->v[1].x,b->v[1].y,b->v[1].z);
				glVertex3f(b->v[2].x,b->v[2].y,b->v[2].z);
				glEnd();
			}
		}
		else
		{
			// Setup view!!!!!!!!
			glLoadIdentity();
			gfx->SetViewport(0, 0, w,h);
			Math::Mat4 prjmat = Math::Mat4::Ortho(0, w, h, 0, -1.0f, 100.0f);
			gfx->SetProjection(&prjmat);

			if (!is_done)
			{
				task_detail_t task[THREAD_COUNT];
				render_result_t result[THREAD_COUNT];
				int tex_indices[THREAD_COUNT];
				for(int threadnum = 0; threadnum < THREAD_COUNT; threadnum++)
				{
					// Render each region
					if (ty < task_count)
					{
						task[threadnum].region[0] = tx*task_size_w;
						task[threadnum].region[1] = ty*task_size_h;
						task[threadnum].region[2] = tx*task_size_w+task_size_w;
						task[threadnum].region[3] = ty*task_size_h+task_size_h;
						task[threadnum].task_id = ty*task_count+tx;

						thread_executor.execute(new RegionTask(kernel, threadnum, &task[threadnum], &blob, &result[threadnum]));
						tex_indices[threadnum] = ty*task_count+tx;

						total_done += 1;
						tx += 1;
						if (tx >= task_count)
						{
							ty += 1;
							tx = 0;
						}
					}
				}

				thread_executor.wait();

				for(int threadnum = 0; threadnum < THREAD_COUNT; threadnum++)
				{
					region_textures[tex_indices[threadnum]] = Pxf::Kernel::GetInstance()->GetGraphicsDevice()->CreateTextureFromData((const unsigned char*)result[threadnum].data, task_size_w, task_size_w, channels);
					region_textures[tex_indices[threadnum]]->SetMagFilter(TEX_FILTER_NEAREST);
					region_textures[tex_indices[threadnum]]->SetMinFilter(TEX_FILTER_NEAREST);
				}
			}

			// Display results
			for(int y = 0; y < task_count; ++y)
			{
				for(int x = 0; x < task_count; ++x)
				{
					if (y*task_count+x < total_done)
					{
						// Bind texture
						Pxf::Kernel::GetInstance()->GetGraphicsDevice()->BindTexture(region_textures[y*task_count+x]);
					}	else {
						Pxf::Kernel::GetInstance()->GetGraphicsDevice()->BindTexture(unfinished_task_texture);
					}
						// Setup quad
						pbatch->QuadsBegin();
						pbatch->QuadsDrawTopLeft(x*task_size_w, y*task_size_h, task_size_w, task_size_w);
						pbatch->QuadsEnd();
				}
			}

			if (total_done == task_count*task_count && !is_done)
			{
				thread_executor.cancel();
				is_done = true;
				render_timer.Stop();
				char title[512];
				Format(title, "Render time: %d ms", render_timer.Interval());
				win->SetTitle(title);
			}
		}
		
		inp->ClearLastKey();
		win->Swap();
	}
	
	gfx->DestroyVertexBuffer(vb);

	delete pbatch;
	
	return 0;
}

