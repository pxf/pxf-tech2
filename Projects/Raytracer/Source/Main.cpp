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
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			blob.primitives[blob.prim_count++] = new Sphere(Math::Vec3f(x-3.5f,-4.f,y+2), .5f, (x+y)%2 == 0 ? sphere_mat1 : sphere_mat2);
		}
	}
	*/

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
	blob.lights[0] = new AreaLight(Pxf::Math::Vec3f(0.0f, 4.8f, 5.0f), 1.0f, 1.0f, Pxf::Math::Vec3f(0.0f, -1.0f, 0.0f), Pxf::Math::Vec3f(1.0f, 0.0f, 0.0f), 5, light_mat1);
	blob.lights[1] = new AreaLight(Pxf::Math::Vec3f(0.0f, -4.8f, 5.0f), 1.0f, 1.0f, Pxf::Math::Vec3f(0.0f, -1.0f, 0.0f), Pxf::Math::Vec3f(1.0f, 0.0f, 0.0f), 5, light_mat1);
	blob.light_count = 2;
	
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
	render_timer.Start();

	bool is_done = false;
	while(win->IsOpen())
	{
		inp->Update();
		if (inp->GetLastKey() == Input::ESC)
			break;
			
		// Setup view!!!!!!!!
		Math::Mat4 prjmat = Math::Mat4::Ortho(0, win->GetWidth(), win->GetHeight(), 0, -0.1f, 100.0f);
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
		
		inp->ClearLastKey();
		win->Swap();
	}
	
	delete pbatch;
	
	return 0;
}

