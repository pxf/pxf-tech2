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
    Resource::Image* img = res->Acquire<Resource::Image>("test.png");

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
    
	Graphics::RenderBuffer* pBuf0 = gfx->CreateRenderBuffer(0,512,512);
	Graphics::FrameBufferObject* pFBO = gfx->CreateFrameBufferObject();
	
	pFBO->AddColorAttachment(pBuf0,0);

    QuadBatch* qb = new QuadBatch(1024);
    qb->Begin();
    qb->SetColor(0.0f, 1.0f, 0.0f);
    qb->AddCentered(0, 0, 1, 1);
    qb->SetColor(1.0f, 0.0f, 0.0f);
    qb->AddCentered(-0.5f, -0.5f, 0.2f, 0.2f);
    qb->End();
    
	glGetIntegerv(0, 0);

    while(win->IsOpen())
    {
        glClear(GL_COLOR_BUFFER_BIT);
        qb->Draw();
        win->Swap();
    }
    
    delete qb;
    

    res->Release(img);
    delete kernel;
    return 0;
}

