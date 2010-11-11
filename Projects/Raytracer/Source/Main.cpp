#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Timer.h>
#include <Pxf/Base/Logger.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Random.h>
#include <Pxf/Base/Memory.h>
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

using namespace Pxf;
using namespace Graphics;

int main(int argc, char* argv[])
{
	Pxf::RandSetSeed(time(NULL));
	Kernel* kernel = Pxf::Kernel::GetInstance();
	kernel->RegisterModule("pri", 0xFFFFFFFF, true);
	
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
	int w = 512;
	int h = 512;
	int channels = 3;
	const int task_count = 8;
	int task_size_w = w / task_count;
	int task_size_h = h / task_count;
	char pixels[w*h*channels];
	
	// job specifics
	batch_blob_t blob;
	blob.pic_w = w;
	blob.pic_h = h;
	
	task_detail_t task;
	task.task_count = task_count*task_count;
	
	// create textures and primitive batches
	Texture *region_textures[task_count*task_count] = {0};
	
	for(int ty = 0; ty < task_count; ++ty)
	{
		for(int tx = 0; tx < task_count; ++tx)
		{
			task.region[0] = tx*task_size_w;
			task.region[1] = ty*task_size_h;
			task.region[2] = tx*task_size_w+task_size_w;
			task.region[3] = ty*task_size_h+task_size_h;
			task.task_id = ty*task_count+tx;
			
			render_result_t pixel_region;
			if (!render_task(&task, &blob, &pixel_region))
			{
				Pxf::Message("Main", "Error while trying to render task: [tx: %d, ty: %d]", tx, ty);
			} else {
				// Success
				region_textures[ty*task_count+tx] = Pxf::Kernel::GetInstance()->GetGraphicsDevice()->CreateTextureFromData((const unsigned char*)pixel_region.data, task_size_w, task_size_w, channels);
			}
		}
	}
	
	PrimitiveBatch *pbatch = new PrimitiveBatch(Pxf::Kernel::GetInstance()->GetGraphicsDevice());
	
	while(win->IsOpen())
	{
		inp->Update();
		if (inp->GetLastKey() == Input::ESC)
			break;
			
		// Setup view!!!!!!!!
		Math::Mat4 prjmat = Math::Mat4::Ortho(0, win->GetWidth(), 0, win->GetHeight(), -0.1f, 100.0f);
	  gfx->SetProjection(&prjmat);
		
		for(int ty = 0; ty < task_count; ++ty)
		{
			for(int tx = 0; tx < task_count; ++tx)
			{
				if (region_textures[ty*task_count+tx])
				{
					// Bind texture
					Pxf::Kernel::GetInstance()->GetGraphicsDevice()->BindTexture(region_textures[ty*task_count+tx]);
					
					// Setup quad
					pbatch->QuadsBegin();
					pbatch->QuadsDrawTopLeft(tx*task_size_w, ty*task_size_h, task_size_w, task_size_w);
					pbatch->QuadsEnd();
				}
			}
		}
		
		inp->ClearLastKey();
		win->Swap();
	}
	
	delete pbatch;
	
	return 0;
}

