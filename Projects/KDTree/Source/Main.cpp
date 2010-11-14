#include <Pxf/Kernel.h>
#include <Pxf/Modules/pri/OpenGL.h>
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
#include <KDTree.h>
#include <cstdlib>

using namespace Pxf;
using namespace Math;

void draw_aabb(const aabb& b)
{
	glBegin(GL_LINES);

	glVertex3f(b.pos.x,b.pos.y,b.pos.z);
	glVertex3f(b.pos.x + b.size.x,b.pos.y,b.pos.z);

	glVertex3f(b.pos.x,b.pos.y,b.pos.z);
	glVertex3f(b.pos.x,b.pos.y + b.size.y,b.pos.z);

	glVertex3f(b.pos.x,b.pos.y,b.pos.z);
	glVertex3f(b.pos.x,b.pos.y,b.pos.z + b.size.z);

	glVertex3f(b.pos.x + b.size.x,b.pos.y,b.pos.z);
	glVertex3f(b.pos.x + b.size.x,b.pos.y + b.size.y,b.pos.z);

	glVertex3f(b.pos.x,b.pos.y + b.size.y,b.pos.z);
	glVertex3f(b.pos.x + b.size.x,b.pos.y + b.size.y,b.pos.z);

	glVertex3f(b.pos.x,b.pos.y,b.pos.z + b.size.z);
	glVertex3f(b.pos.x,b.pos.y + b.size.y,b.pos.z + b.size.z);

	glVertex3f(b.pos.x,b.pos.y,b.pos.z + b.size.z);
	glVertex3f(b.pos.x + b.size.x,b.pos.y,b.pos.z + b.size.z);

	// ETC..

	glEnd();
}

void DrawTree(KDTree* t)
{
	KDNode* root = t->GetRoot();

	draw_aabb(t->GetAABB());


}

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
	
	srand ( time(NULL) );

	// KDTree
	KDTree* tree = new KDTree(2);

	Primitive pData[3];

	pData[0] = Primitive(Vec3f(-200.0,-50.0f,-150.0f),Vec3f(-150.0f,-50.0f,-150.0f),Vec3f(-150.0f,0.0f,-150.0f));
	pData[1] = Primitive(Vec3f(-50.0f,50.0f,0),Vec3f(-50.0f,100.0f,0),Vec3f(-100.0f,100.0f,100.0f));
	pData[2] = Primitive(Vec3f(150.0f,-50.0f,0),Vec3f(150.0f,-100.0f,0),Vec3f(200.0f,-100.0f,100.0f));

	tree->Build(pData,3);

	while(win->IsOpen())
	{
		inp->Update();
		if (inp->GetLastKey() == Input::ESC)
			break;
		
		// Setup ogl
		gfx->SetViewport(0, 0, win->GetWidth(), win->GetHeight());
		Math::Mat4 prjmat = Math::Mat4::Ortho(-300.0f, 300.0f, 300.0f,-300.0f, -1.0f, 10000.0f);
		gfx->SetProjection(&prjmat);
		glClearColor(26.0f/255.0f,26.0f/255.0f,26.0f/255.0f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glColor3f(1.0f,1.0f,1.0f);

		DrawTree(tree);

		for(size_t i = 0; i < 3; i++)
		{
			draw_aabb(pData[i].GetAABB());
		}
			
		inp->ClearLastKey();
		win->Swap();
	}
	
	return 0;
}

