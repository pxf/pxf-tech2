#include <stdio.h>

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>
#include <Pxf/Base/Timer.h>

#include <Pxf/Audio/AudioDevice.h>
#include <Pxf/Input/InputDevice.h>
#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Graphics/Window.h>
#include <Pxf/Graphics/WindowSpecifications.h>
#include <Pxf/Graphics/RenderBuffer.h>
#include <Pxf/Graphics/Model.h>

#include <Pxf/Graphics/VertexBuffer.h>
#include <Pxf/Graphics/VertexBufferDefs.h>

#include <Pxf/Base/Hash.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Memory.h>
#include <Pxf/Base/Random.h>

#include <Pxf/Modules/pri/OpenGL.h>
#include <Pxf/Modules/pri/UniGL.h>

#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/ResourceLoader.h>
#include <Pxf/Resource/Image.h>
#include <Pxf/Resource/Sound.h>
#include <Pxf/Resource/Font.h>
#include <Pxf/Resource/Text.h>

#include <Pxf/Graphics/FrameBufferObject.h>

#include "SimpleQuad.h"

#include <ctime>

using namespace Pxf;
using namespace Math;
using namespace Graphics;
using namespace Derp;


int main()
{
	Pxf::RandSetSeed(time(NULL));
	Kernel* kernel = Pxf::Kernel::GetInstance();

	// Just load everything
	kernel->RegisterModule("pri", 0xFFFF, true);
	kernel->RegisterModule("img", 0xFFFF, true);
	kernel->RegisterModule("mesh", 0xFFFF, true);
	kernel->RegisterModule("net", 0xFFFF, true);
	kernel->DumpAvailableModules();

	Graphics::GraphicsDevice* gfx = kernel->GetGraphicsDevice();
	Input::InputDevice* inp = kernel->GetInputDevice();
	Resource::ResourceManager* res = kernel->GetResourceManager();
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
	
	// Setup full screen quad
	SimpleQuad* finalquad = new SimpleQuad(0.0f, 0.0f, 1.0f, 1.0f);

	Graphics::Texture* a = gfx->CreateEmptyTexture(512,512);
	Graphics::Texture* b = gfx->CreateEmptyTexture(512,512);

	Graphics::FrameBufferObject* FBO = gfx->CreateFrameBufferObject();
	FBO->Attach(a,GL_COLOR_ATTACHMENT0_EXT, false);
	FBO->Attach(b,GL_COLOR_ATTACHMENT1_EXT, false);

	const char* vs = "void main(void)  \
                  {					\
                  	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; \
                  	gl_TexCoord[0] = gl_MultiTexCoord0; \
                  }"; 

	const char* fs = "void main() \
                  { \
                  	gl_FragData[0] = vec4(0.0, 1.0, 0.0, 1.0); \
                  	gl_FragData[1] = vec4(1.0, 0.0, 0.0, 1.0); \
                  }";
	const char* vs_res = "uniform sampler2D texa; \
                  uniform sampler2D texb; \
                  void main(void) \
                  { \
                  	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; \
                  	gl_TexCoord[0] = gl_MultiTexCoord0; \
                  }";

	const char* fs_res = "uniform sampler2D texa; \
                  uniform sampler2D texb; \
                  void main() \
                  { \
                    if (gl_TexCoord[0].t > 0.5) { \
                        gl_FragColor = texture2D(texb, gl_TexCoord[0].st); \
					} else { \
                        gl_FragColor = texture2D(texa, gl_TexCoord[0].st); \
					} \
                  }";

	Graphics::Shader* shader = gfx->CreateShader("TEST",vs,fs);
	Graphics::Shader* result = gfx->CreateShader("RESULT",vs_res,fs_res);
	gfx->SetUniformi(result, "texa", 0);
	gfx->SetUniformi(result, "texb", 1);

	Timer t;
	while(win->IsOpen())
	{
		t.Start();	
		
		// Setup ogl
		gfx->SetViewport(0, 0, win->GetWidth(), win->GetHeight());
		Math::Mat4 prjmat = Math::Mat4::Ortho(0, 1.0f, 1.0f, 0, -1.0f, 10000.0f);
		gfx->SetProjection(&prjmat);
		glClearColor(26.0f/255.0f,26.0f/255.0f,26.0f/255.0f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Render final output
		glEnable(GL_TEXTURE_2D);

		gfx->BindFrameBufferObject(FBO);
		gfx->BindShader(shader);

		finalquad->Draw();
		gfx->BindShader(0);

		gfx->UnbindFrameBufferObject();
	
		//glBindTexture(GL_TEXTURE_2D,((Modules::RenderBufferGL2*) a)->GetHandle());
		gfx->BindTexture(b ,1);
		gfx->BindTexture(a, 0);

		gfx->BindShader(result);
		finalquad->Draw();

		inp->Update();
		
		if (inp->GetLastKey() == Input::ESC)
			break;
		
		inp->ClearLastKey();
		inp->ClearLastButton();
		
		char title[512];
		t.Stop();
		Format(title, "Renderer (fps: %d, processing time: %d ms)", win->GetFPS(), t.Interval());
		win->SetTitle(title); 

		win->Swap();
	}
	
	delete finalquad;

	delete kernel;
	return 0;
}

