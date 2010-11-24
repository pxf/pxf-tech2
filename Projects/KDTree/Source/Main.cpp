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
#include <Intersections.h>
#include <cstdlib>

using namespace Pxf;
using namespace Math;

Vec3f w(1.0f,1.0f,1.0f);
Vec3f red(1.0f,0.0f,0.0f);

struct mouse_state 
{
	enum MS {
		NO_STATE,
		LEFT,
		RIGHT,
		LEFT_RIGHT
	};

	Math::Vec2i last_pos;
	MS state;
} ms;

void draw_aabb(const aabb& b,Vec3f c = w)
{
	glBegin(GL_LINES);

	glColor3f(c.r,c.g,c.b);

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

	glVertex3f(b.pos.x + b.size.x,b.pos.y,b.pos.z);
	glVertex3f(b.pos.x + b.size.x,b.pos.y,b.pos.z + b.size.z);

	glVertex3f(b.pos.x,b.pos.y + b.size.y,b.pos.z);
	glVertex3f(b.pos.x,b.pos.y + b.size.y,b.pos.z + b.size.z);

	glVertex3f(b.pos.x + b.size.x,b.pos.y,b.pos.z + b.size.z);
	glVertex3f(b.pos.x + b.size.x,b.pos.y + b.size.y,b.pos.z + b.size.z);

	glVertex3f(b.pos.x + b.size.x,b.pos.y + b.size.y,b.pos.z);
	glVertex3f(b.pos.x + b.size.x,b.pos.y + b.size.y,b.pos.z + b.size.z);

	glVertex3f(b.pos.x,b.pos.y + b.size.y,b.pos.z + b.size.z);
	glVertex3f(b.pos.x + b.size.x,b.pos.y + b.size.y,b.pos.z + b.size.z);

	glColor3f(1.0f,1.0f,1.0f);

	glEnd();
}

void DrawPrimitive(Primitive* _Data,Vec3f c = w)
{
	if(_Data)
	{
		if(_Data->GetType() == Primitive::TRIANGLE)
		{
			Vec3f* p = _Data->GetPoints();

			glBegin(GL_TRIANGLES);

			glColor3f(c.x,c.y,c.z);
			glVertex3f(p[0].x,p[0].y,p[0].z);
			glVertex3f(p[1].x,p[1].y,p[1].z);
			glVertex3f(p[2].x,p[2].y,p[2].z);

			glColor3f(1.0f,1.0f,1.0f);

			glEnd();
		}
	}
}

void DrawRay(ray_t& _R)
{
	Vec3f o = _R.o;
	Vec3f d = _R.d;
	Vec3f p = o + d * 1000.0f;

	glBegin(GL_LINES);

	glVertex3f(o.x,o.y,o.z);
	glVertex3f(p.x,p.y,p.z);

	glEnd();
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

		
		glVertex3f(_Node->GetSplitPos(),lb.pos.y,lb.pos.z + lb.size.z);
		glVertex3f(_Node->GetSplitPos(),lb.pos.y + lb.size.y,lb.pos.z + lb.size.z);

		
		glVertex3f(_Node->GetSplitPos(),lb.pos.y + lb.size.y,lb.pos.z);
		glVertex3f(_Node->GetSplitPos(),lb.pos.y + lb.size.y,lb.pos.z + lb.size.z);

		glVertex3f(_Node->GetSplitPos(),lb.pos.y,lb.pos.z);
		glVertex3f(_Node->GetSplitPos(),lb.pos.y,lb.pos.z + lb.size.z);

		glEnd();
		glColor3f(1.0f,1.0f,1.0f);
	}
	else if(axis == 1)
	{
		glColor3f(0.0f,1.0f,0.0f);
		glBegin(GL_LINES);

		glVertex3f(lb.pos.x,_Node->GetSplitPos(),lb.pos.z);
		glVertex3f(lb.pos.x + lb.size.x,_Node->GetSplitPos(),lb.pos.z);

		glVertex3f(lb.pos.x,_Node->GetSplitPos(),lb.pos.z + lb.size.z);
		glVertex3f(lb.pos.x + lb.size.x,_Node->GetSplitPos(),lb.pos.z + lb.size.z);

		glVertex3f(lb.pos.x,_Node->GetSplitPos(),lb.pos.z);
		glVertex3f(lb.pos.x,_Node->GetSplitPos(),lb.pos.z + lb.size.z);

		glVertex3f(lb.pos.x + lb.size.x,_Node->GetSplitPos(),lb.pos.z);
		glVertex3f(lb.pos.x + lb.size.x,_Node->GetSplitPos(),lb.pos.z + lb.size.z);

		glEnd();
		glColor3f(1.0f,1.0f,1.0f);
	}
	else if(axis == 2)
	{
		glColor3f(0.0f,0.0f,1.0f);
		glBegin(GL_LINES);
		
		glVertex3f(lb.pos.x,lb.pos.y,_Node->GetSplitPos());
		glVertex3f(lb.pos.x + lb.size.x,lb.pos.y,_Node->GetSplitPos());

		glVertex3f(lb.pos.x,lb.pos.y + lb.size.y,_Node->GetSplitPos());
		glVertex3f(lb.pos.x + lb.size.x,lb.pos.y + lb.size.y,_Node->GetSplitPos());

		glVertex3f(lb.pos.x,lb.pos.y,_Node->GetSplitPos());
		glVertex3f(lb.pos.x,lb.pos.y + lb.size.y,_Node->GetSplitPos());

		glVertex3f(lb.pos.x + lb.size.x,lb.pos.y,_Node->GetSplitPos());
		glVertex3f(lb.pos.x + lb.size.x,lb.pos.y + lb.size.y,_Node->GetSplitPos());

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
		Vec3f pos = cd.cam->GetDir();
		Vec3f pp_vec;
		pp_vec.x = -pos.z;
		pp_vec.z = pos.x;
		pp_vec.y = pos.y;

		pp_vec = pp_vec * -dpos.x;

		cd.cam->Translate(pp_vec.x,dpos.y,pp_vec.z); 

		ms.state = mouse_state::LEFT;
	}
	else if(cd.inp->IsButtonDown(Pxf::Input::MouseButton::MOUSE_RIGHT) && !cd.inp->IsButtonDown(Pxf::Input::MouseButton::MOUSE_LEFT))
	{		
		cd.cam->SetRotation(dpos.x * 0.5f,dpos.y * 0.5f,0.0f);
		ms.state = mouse_state::RIGHT;
	}
	else if(cd.inp->IsButtonDown(Pxf::Input::MouseButton::MOUSE_LEFT) && cd.inp->IsButtonDown(Pxf::Input::MouseButton::MOUSE_RIGHT))
	{
		Math::Vec3f ed = cd.cam->GetDir();

		cd.cam->Translate((ed)*dpos.y);
		ms.state = mouse_state::LEFT_RIGHT;
	}
	else
		ms.state = mouse_state::NO_STATE;

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
	KDTree* tree = new KDTree(2);

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
	Math::Mat4 prjmat = Math::Mat4::Perspective(45.0f, win->GetWidth() / win->GetHeight(), 1.0f,10000.0f); // (-300.0f, 300.0f, 300.0f,-300.0f, 1.0f, 100000.0f);

	cam.SetProjectionView(prjmat);

	cam.Translate(0.0f,50.0f,500.0f);

	camera_input_desc cid;
	cid.cam = &cam;
	cid.inp = inp;

	float a = 0.0f;

	// triangle intersection test
	Vec3f triangle[3];
	triangle[0] = Vec3f(-5.0f,-5.0f,5.0f);
	triangle[1] = Vec3f(5.0f,-5.0f,0.0f);
	triangle[2] = Vec3f(0.0f,5.0f,-5.0f);

	ray_t r;
	r.o = Vec3f(0.0f,0.0f,0.0f);
	r.d = Vec3f(0.0f,0.0f,1.0f);

	intersection_response_t resp;

	Primitive p(triangle[0],triangle[1],triangle[2]);

	aabb p_aabb = p.GetAABB();

	//void *ptr = new void[size];
	//void **ptr = new void*[3];
	//void* (*cPtr)() = (*(__cdecl *)(void*)Create<Primitive>);

	Primitive* (*ptr)() = &Create<Primitive>;
	void* __cdecl _ptr[1] = { ptr };//reinterpret_cast<void*>(ptr) };

	Primitive* lolsp = (*ptr)();

	//(*_ptr[0])();

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

		MoveCamera(cid);

		//r.o = cam.GetPos() + Vec3f(0.1f,0.1f,0.1f);
		//r.d = cam.GetDir();

		DrawTree(tree);
		draw_aabb(tree->GetRoot()->GetAABB());

		ray_t test_ray;
		test_ray.o = Vec3f(300.0f,-30.0f,-130.0f);
		test_ray.d = Vec3f(-1.0f,0.0f,0.0f);
		Normalize(test_ray.d);
		test_ray.inv_d = Inverse(test_ray.d);
		
		DrawRay(test_ray);
		Primitive* p_result = RayTreeIntersect((*tree),test_ray,10000.0f);

		DrawPrimitive(p_result,red);

		inp->ClearLastKey();
		win->Swap();
	}
	
	delete tree;
	//delete pData;

	return 0;
}

