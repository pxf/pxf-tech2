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
#include <Pxf/Input/InputDevice.h>
#include <ctime>
#include <cstdio>
#include <cstdlib>

using namespace Pxf;

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
	
	while(win->IsOpen())
	{
		inp->Update();
		if (inp->GetLastKey() == Input::ESC)
			break;
			
		inp->ClearLastKey();
		win->Swap();
	}
	
	return 0;
}

