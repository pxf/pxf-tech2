#include <Pxf/Kernel.h>
#include <Pxf/Base/Path.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Hash.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Memory.h>
#include <Pxf/Base/Random.h>
#include <Pxf/Base/Timer.h>
#include <Pxf/Base/Logger.h>
#include <Pxf/Base/Platform.h>
#include <Pxf/Input/InputDevice.h>
#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Graphics/Font.h>
#include <Pxf/Graphics/Window.h>
#include <Pxf/Graphics/WindowSpecifications.h>
#include <Pxf/Graphics/VertexBuffer.h>
#include <Pxf/Graphics/VertexBufferDefs.h>
#include <Pxf/Graphics/PrimitiveBatch.h>
#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/ResourceLoader.h>
#include <Pxf/Resource/Image.h>
#include <Pxf/Resource/Json.h>
#include <Pxf/Resource/Font.h>
#include <Pxf/Modules/pri/OpenGL.h>
#include <Gui.h>

#include <ctime>

using namespace Pxf;
using namespace Math;
using namespace Graphics;

int main()
{
	Pxf::RandSetSeed(time(NULL));
	Kernel* kernel = Pxf::Kernel::GetInstance();

	unsigned main_tag = kernel->CreateTag("Main");

	// Just load everything
	kernel->RegisterModule("pri", 0xFFFF, true);
	kernel->RegisterModule("img", 0xFFFF, true);
	kernel->RegisterModule("mesh", 0xFFFF, true);
	kernel->RegisterModule("json", 0xFFFF, true);
	kernel->DumpAvailableModules();

	Graphics::GraphicsDevice* gfx = kernel->GetGraphicsDevice();
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
			kernel->Log(main_tag | Logger::IS_INFORMATION, "Saving new config!");
		}
	}

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

	Math::Mat4 prjmat = Math::Mat4::Ortho(0, spec.Width, spec.Height, 0, -10, 10);
	gfx->SetProjection(&prjmat);
	gfx->SetViewport(0, 0, spec.Width, spec.Height);

	Graphics::PrimitiveBatch* pb = new Graphics::PrimitiveBatch(gfx);

	Resource::Font* bitmapfont = res->Acquire<Resource::Font>("data/consolas28.pfnt");
	Graphics::Font* fnt = new Graphics::Font(gfx, bitmapfont);
	//fnt->SetTextureFilters(TEX_FILTER_NEAREST, TEX_FILTER_NEAREST); // <- crisp font

	// Stack objects must be destroyed before deleting the kernel
	{
		Gui gui(kernel);

		Timer processing_time;
		while(win->IsOpen())
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(0, 0, 0, 1);
			processing_time.Start();

			inp->Update();
			gui.Update();
			
			if (inp->GetLastKey() == Input::ESC)
				break;

			//gfx->Print(fnt, 100, 100, 0.5f + fabs(sinf(Platform::GetTime()/1000.f)), "test apa bepa !.,?-//=");
			
			{
				static Graphics::Rect::Rect_t rect = {300, 0, 200, 50};
				if (Gui::INPUT_ACTION & gui.DoButton((void*)'KNP1', &rect, "Quit", 0, 0))
				{
					break;
				}
			}

			{
				static Graphics::Rect::Rect_t rect = {300, 60, 200, 50};
				if (Gui::INPUT_ACTION & gui.DoButton((void*)'KNP2', &rect, "Quit", 0, 0))
				{
					break;
				}
			}


			inp->ClearLastKey();
			inp->ClearLastButton();
			
			char title[512];
			processing_time.Stop();
			Format(title, "Renderer (fps: %d, processing time: %d ms)", win->GetFPS(), processing_time.Interval());
			win->SetTitle(title);

			win->Swap();
		}

}
	delete kernel;
	return 0;
}

