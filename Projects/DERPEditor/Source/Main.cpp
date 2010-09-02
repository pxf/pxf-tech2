//#include <Pxf/SampleModule/SampleModule.h>
#include <stdio.h>

#include <Pxf/Kernel.h>
#include <Pxf/Base/Debug.h>
#include <Pxf/Base/Utils.h>

#include <Pxf/Audio/AudioDevice.h>
#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Graphics/Window.h>
#include <Pxf/Graphics/WindowSpecifications.h>
#include <Pxf/Graphics/RenderBuffer.h>
#include <Pxf/Graphics/FrameBufferObject.h>

#include <Pxf/Base/Hash.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Memory.h>
#include <Pxf/Base/Random.h>

#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/ResourceLoader.h>
#include <Pxf/Resource/Image.h>
#include <Pxf/Resource/Blob.h>

#include <enet/enet.h>

#include <ctime>

#include "QuadBatch.h"
#include "TexturedQuadBatch.h"
#include <Pxf/Modules/pri/OpenGL.h>

using namespace Pxf;
using namespace DERPEditor;

int main()
{
    Pxf::RandSetSeed(time(NULL));
    Kernel* kernel = Pxf::Kernel::GetInstance();

    kernel->RegisterModule("pri", Pxf::System::SYSTEM_TYPE_GRAPHICSDEVICE, true);
    kernel->RegisterModule("img", Pxf::System::SYSTEM_TYPE_RESOURCE_LOADER, true);
    kernel->DumpAvailableModules();

    Graphics::GraphicsDevice* gfx = kernel->GetGraphicsDevice();
    Audio::AudioDevice* snd = kernel->GetAudioDevice();
    Input::InputDevice* inp = kernel->GetInputDevice();
    Resource::ResourceManager* res = kernel->GetResourceManager();
    //Resource::Image* img = res->Acquire<Resource::Image>("test.png");

    res->DumpResourceLoaders();

    snd->Play(2);

    Resource::BlobLoader* loader = res->FindResourceLoader<Resource::BlobLoader>("blob");
    Resource::Blob* blob = loader->CreateFrom("aoeu", 5);

    /*
    // Resource::BinaryFile to complement? Resource::Blob, binary large object
    Resource::TextFile* shadersrc = res->Acquire<Resource::TextFile>("fiddle.glsl");
    Graphics::Shader* shader = video->CreateShader(Resource::TextFile* _source);
    shader->SetBlahBlah();
    */

    Graphics::WindowSpecifications spec;
    spec.Width = 800;
    spec.Height = 600;
    spec.ColorBits = 24;
    spec.AlphaBits = 8;
    spec.DepthBits = 8;
    spec.StencilBits = 0;
    spec.FSAASamples = 0;
    spec.Fullscreen = false;
    spec.Resizeable = false;
    spec.VerticalSync = false;
    
    Graphics::Window* win = gfx->OpenWindow(&spec);
   

    // FBO tests
	Graphics::Texture* tex0 = gfx->CreateEmptyTexture(spec.Width, spec.Height);
	Graphics::RenderBuffer* pBuf0 = gfx->CreateRenderBuffer(0,512,512);
	Graphics::FrameBufferObject* pFBO = gfx->CreateFrameBufferObject();
	
	pFBO->AddColorAttachment(tex0,0,true);
	pFBO->AddColorAttachment(tex0,0,true);

	pFBO->AddDepthAttachment(pBuf0);

	printf("Color attachments: %i\n",pFBO->GetNumColorAttachment());
	
	// QuadBatch tests
	glEnable( GL_TEXTURE_2D );

	QuadBatch* testFBO = new QuadBatch(4);
	testFBO->Begin();
	testFBO->AddTopLeft(0, 0, spec.Width, spec.Height);
	testFBO->SetColor(0.0f,1.0f,0.0f);
	testFBO->End();

    Math::Mat4 transform = Math::Mat4::Identity;
    TexturedQuadBatch* qb = new TexturedQuadBatch(1024, &transform, "data/test.png");
    
    qb->Begin();
    qb->AddCentered(400, 400, 100, 100);
    qb->SetColor(1.0f, 0.0f, 0.0f);
    transform.Translate(-200.0f, 0.0f, 0.0f);
    qb->AddCentered(400, 400, 50, 50);
    qb->End();
    
    // Setup viewport and matrises
    gfx->SetViewport(0, 0, 800, 600);
    Math::Mat4 prjmat = Math::Mat4::Ortho(0, 800, 600, 0, -1000.0f, 1000.0f);
    gfx->SetProjection(&prjmat);

    while(win->IsOpen())
    {
        glClear(GL_COLOR_BUFFER_BIT);

		gfx->BindFrameBufferObject(pFBO);
        qb->Draw();
		gfx->UnbindFrameBufferObject();

		gfx->BindTexture(tex0);

		testFBO->Draw();

        win->Swap();
    }
    
    delete qb;
    

    //res->Release(img);
    delete kernel;
    return 0;
}

