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
#include <Camera.h>
#include <cstdlib>

using namespace Pxf;
using namespace Math;


struct mouse_state 
{
	Math::Vec2i last_pos;
} ms;

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

void DrawPrimitive(Primitive* _Data)
{
	if(_Data)
	{
		if(_Data->GetType() == Primitive::TRIANGLE)
		{
			Vec3f* p = _Data->GetPoints();

			glBegin(GL_TRIANGLES);

			glVertex3f(p[0].x,p[0].y,p[0].z);
			glVertex3f(p[1].x,p[1].y,p[1].z);
			glVertex3f(p[2].x,p[2].y,p[2].z);

			glEnd();
		}
	}
}

void DrawNode(KDNode* _Node)
{
	if(!_Node) return;

	unsigned axis = _Node->GetAxis();
	aabb lb = _Node->GetAABB();

	if(axis == 0)
	{
		glColor3f(1.0f,0.0f,0.0f);
		glBegin(GL_LINES);
		glVertex3f(_Node->GetSplitPos(),lb.pos.y,lb.pos.z);
		glVertex3f(_Node->GetSplitPos(),lb.pos.y + lb.size.y,lb.pos.z);
		glEnd();
		glColor3f(1.0f,1.0f,1.0f);
	}
	else if(axis == 1)
	{
		glColor3f(0.0f,1.0f,0.0f);
		glBegin(GL_LINES);
		glVertex3f(lb.pos.x,_Node->GetSplitPos(),lb.pos.z);
		glVertex3f(lb.pos.x + lb.size.x,_Node->GetSplitPos(),lb.pos.z);
		glEnd();
		glColor3f(1.0f,1.0f,1.0f);
	}

	KDNode* left = _Node->GetLeftChild();
	KDNode* right = _Node->GetRightChild();

	if(left)
	{
		if(left->IsLeaf() && !left->IsEmpty())
			DrawPrimitive(left->GetPrimitiveData());
		else
			DrawNode(left);
	}

	if(right)
	{
		if(right->IsLeaf() && !right->IsEmpty())
			DrawPrimitive(right->GetPrimitiveData());
		else
			DrawNode(right);
	}
}

void DrawTree(KDTree* t)
{
	KDNode* root = t->GetRoot();

	draw_aabb(root->GetAABB());

	DrawNode(root);
}

struct camera_input_desc {
	SimpleCamera* cam;
	Input::InputDevice* inp;
};


void MoveCamera(const camera_input_desc& cd)
{
	// Mouse movement:

	Math::Vec2i dpos;
	cd.inp->GetMousePos(&dpos.x,&dpos.y);
	dpos = dpos - ms.last_pos;

	if(cd.inp->IsButtonDown(Pxf::Input::MouseButton::MOUSE_LEFT) && !cd.inp->IsButtonDown(Pxf::Input::MouseButton::MOUSE_RIGHT))
	{
		cd.cam->Translate(-dpos.x,-dpos.y,0);
	}
	else if(cd.inp->IsButtonDown(Pxf::Input::MouseButton::MOUSE_RIGHT) && !cd.inp->IsButtonDown(Pxf::Input::MouseButton::MOUSE_LEFT))
	{		
		cd.cam->SetRotation(-dpos.x,dpos.y,0.0f);
	}
	else if(cd.inp->IsButtonDown(Pxf::Input::MouseButton::MOUSE_LEFT) && cd.inp->IsButtonDown(Pxf::Input::MouseButton::MOUSE_RIGHT))
	{
		cd.cam->Translate(0.0f,0.0f,dpos.y);
	}

	// update mouse state
	cd.inp->GetMousePos(&ms.last_pos.x,&ms.last_pos.y);
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
	KDTree* tree = new KDTree(3);

	Primitive pData[5];

	pData[0] = Primitive(Vec3f(-200.0,-50.0f,50.0f),Vec3f(-150.0f,-50.0f,-150.0f),Vec3f(-150.0f,0.0f,-150.0f));
	pData[1] = Primitive(Vec3f(-50.0f,50.0f,0.0f),Vec3f(-50.0f,100.0f,0),Vec3f(-100.0f,100.0f,100.0f));
	pData[2] = Primitive(Vec3f(150.0f,-50.0f,0.0f),Vec3f(150.0f,-100.0f,0),Vec3f(200.0f,-100.0f,100.0f));
	pData[3] = Primitive(Vec3f(50.0f,10.0f,0),Vec3f(50.0f,60.0f,0),Vec3f(100.0f,60.0f,100.0f));
	pData[4] = Primitive(Vec3f(130.0f,50.0f,0),Vec3f(130.0f,50.0f,0),Vec3f(180.0f,100.0f,100.0f));

	/*
	pData[0] = Primitive(Vec3f(-200.0,-50.0f,0.0f),Vec3f(-150.0f,-50.0f,0.0f),Vec3f(-150.0f,0.0f,0.0f));
	pData[1] = Primitive(Vec3f(-50.0f,50.0f,0.0f),Vec3f(-50.0f,100.0f,0.0f),Vec3f(-100.0f,100.0f,0.0f));
	pData[2] = Primitive(Vec3f(150.0f,-50.0f,0.0f),Vec3f(150.0f,-100.0f,0.0f),Vec3f(200.0f,-100.0f,0.0f));
	pData[3] = Primitive(Vec3f(50.0f,10.0f,0.0f),Vec3f(50.0f,60.0f,0.0f),Vec3f(100.0f,60.0f,0.0f));
	//pData[4] = Primitive(Vec3f(130.0f,50.0f,0.0f),Vec3f(130.0f,50.0f,0.0f),Vec3f(180.0f,100.0f,0.0f)); */

	tree->Build(pData,4);

	PrintStatistics(tree);

	SimpleCamera cam;
	//cam.SetPerspective(45.0f, win->GetWidth() / win->GetHeight(),-1.0f,1000.0f);

	// Setup ogl
	gfx->SetViewport(0, 0, win->GetWidth(), win->GetHeight());
	Math::Mat4 prjmat = Math::Mat4::Ortho(-300.0f, 300.0f, 300.0f,-300.0f, 1.0f, 100000.0f);

	cam.SetProjectionView(prjmat);

	cam.Translate(0.0f,50.0f,0.0f);

	camera_input_desc cid;
	cid.cam = &cam;
	cid.inp = inp;

	float a = 0.0f;

	while(win->IsOpen())
	{
		inp->Update();
		if (inp->GetLastKey() == Input::ESC)
			break;

		gfx->SetProjection(cam.GetProjectionView());
		gfx->SetModelView(cam.GetModelView());

		glClearColor(26.0f/255.0f,26.0f/255.0f,26.0f/255.0f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glColor3f(1.0f,1.0f,1.0f);

		//MoveCamera(cid);

		glRotatef(a,0.0f,1.0f,0.0f);

		DrawTree(tree);
			
		a += 0.01f;

		inp->ClearLastKey();
		win->Swap();
	}
	
	delete tree;
	//delete pData;

	return 0;
}

