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
#include <Pxf/Graphics/Shader.h>

#include <Pxf/Base/Hash.h>
#include <Pxf/Base/String.h>
#include <Pxf/Base/Memory.h>
#include <Pxf/Base/Random.h>

#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/ResourceLoader.h>
#include <Pxf/Resource/Image.h>
#include <Pxf/Resource/Mesh.h>
#include <Pxf/Resource/Blob.h>

#include <ctime>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#include "QuadBatch.h"
#include <Pxf/Modules/pri/OpenGL.h>

using namespace Pxf;

extern int new_BakeFontBitmap(const unsigned char *data, int offset,  // font location (use offset=0 for plain .ttf)
                                float pixel_height,                     // height of font in pixels
                                int pixel_blocksize,
                                unsigned char *pixels, int pw, int ph,  // bitmap to be filled in
                                int first_char, int num_chars,          // characters to bake
                                stbtt_bakedchar *chardata)
{
   float scale;
   int x,y,bottom_y, i;
   stbtt_fontinfo f;
   stbtt_InitFont(&f, data, offset);
   STBTT_memset(pixels, 0, pw*ph); // background of 0 around pixels
   x=y=0;
   

   scale = stbtt_ScaleForPixelHeight(&f, pixel_height);

   for (i=0; i < num_chars; ++i) {
      int advance, lsb, x0,y0,x1,y1,gw,gh;
      int g = stbtt_FindGlyphIndex(&f, first_char + i);
      stbtt_GetGlyphHMetrics(&f, g, &advance, &lsb);
      stbtt_GetGlyphBitmapBox(&f, g, scale,scale, &x0,&y0,&x1,&y1);
      /*gw = x1-x0;
      gh = y1-y0;*/
      float w2,h2;
      w2 = pixel_blocksize / 2.0f;//(x1-x0) / 2.0f;
      h2 = pixel_blocksize / 2.0f;//(y1-y0) / 2.0f;
      gw = pixel_blocksize;// - w2;
      gh = pixel_blocksize;// - h2;
      float glw,glh;
      glw = pixel_blocksize / 4.0f;
      glh = y0;
      if (x >= pw / pixel_blocksize)
      {
        x = 0;
        y += 1;
      }
      /*if (x + gw >= pw)
         y = bottom_y, x = 0; // advance to next row
         */
      if (y >= ph / pixel_blocksize) // check if it fits vertically AFTER potentially moving to next row
         return -i;
      STBTT_assert(x+gw < pw);
      STBTT_assert(y+gh < ph);
      stbtt_MakeGlyphBitmap(&f, pixels+(unsigned char)(w2 + x*pixel_blocksize - glw)+(unsigned char)(h2 + y*pixel_blocksize - glh)*pw, gw,gh,pw, scale,scale, g);
      chardata[i].x0 = (stbtt_int16) w2 + x*pixel_blocksize - glw;
      chardata[i].y0 = (stbtt_int16) h2 + y*pixel_blocksize - glh;
      chardata[i].x1 = (stbtt_int16) (w2 + x*pixel_blocksize + gw - glw);
      chardata[i].y1 = (stbtt_int16) (h2 + y*pixel_blocksize + gh - glh);
      chardata[i].xadvance = pixel_blocksize;//scale * advance;
      chardata[i].xoff     = 0;//(float) x0;
      chardata[i].yoff     = (float) y0;
      /*x = x + gw + 2;
      if (y+gh+2 > bottom_y)
         bottom_y = y+gh+2;*/
      x += 1;
   }
   return bottom_y;
}

void gen_bitmapdata(unsigned char* _outdata, int _texsize, float _fontsize, float _blocksize, const char* _fontpath)
{
  unsigned char ttf_buffer[1<<20];

  uint char_range[2];
  char_range[0] = 32;
  char_range[1] = 256;

  stbtt_bakedchar cdata[char_range[1] - char_range[0]]; // ASCII 32..126 is 95 glyphs

  fread(ttf_buffer, 1, 1<<20, fopen(_fontpath, "rb"));
  new_BakeFontBitmap(ttf_buffer,0, _fontsize, _blocksize, _outdata, _texsize, _texsize, char_range[0],char_range[1] - char_range[0], cdata); // no guarantee this fits!
}

int main(int argc, char** argv)
{
    Pxf::RandSetSeed(time(NULL));
    Kernel* kernel = Pxf::Kernel::GetInstance();

    kernel->RegisterModule("pri", Pxf::System::SYSTEM_TYPE_GRAPHICSDEVICE, true);
    kernel->RegisterModule("img", Pxf::System::SYSTEM_TYPE_RESOURCE_LOADER, true);
    kernel->DumpAvailableModules();

    Graphics::GraphicsDevice* gfx = kernel->GetGraphicsDevice();
    Resource::ResourceManager* res = kernel->GetResourceManager();
    res->DumpResourceLoaders();
    
    int texsize = 512;
    float fontsize = 12;
    float blocksize = 16;
    if (argc >= 1)
      texsize = atoi(argv[1]);
    if (argc >= 2)
      fontsize = atof(argv[2]);
    if (argc >= 3)
      blocksize = atof(argv[3]);

    Graphics::WindowSpecifications spec;
    spec.Width = texsize;
    spec.Height = texsize;
    spec.ColorBits = 24;
    spec.AlphaBits = 8;
    spec.DepthBits = 24;
    spec.StencilBits = 0;
    spec.FSAASamples = 0;
    spec.Fullscreen = false;
    spec.Resizeable = false;
    spec.VerticalSync = false;
    
    Graphics::Window* win = gfx->OpenWindow(&spec);

    // FBO tests
  	Graphics::Texture* tex0 = gfx->CreateEmptyTexture(texsize, texsize);
  	Graphics::RenderBuffer* pBuf0 = gfx->CreateRenderBuffer(GL_DEPTH_COMPONENT,texsize,texsize);
  	Graphics::FrameBufferObject* pFBO = gfx->CreateFrameBufferObject();
	
    pFBO->Attach(tex0,GL_COLOR_ATTACHMENT0,true);
    
    float tdepth = 1.0f;
    Math::Vec4f tcolor = Math::Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
    QuadBatch* outFBO = new QuadBatch(4, &tdepth, &tcolor);
  	outFBO->Begin();
  	outFBO->AddTopLeft(0, 0, texsize, texsize);
  	outFBO->End();
  	
  	QuadBatch* qb = new QuadBatch(1024, &tdepth, &tcolor);
  	//qb->Begin();
  	//qb->AddTopLeft(20, 20, 50, 50);
  	//qb->End();

  	// OGL setups
  	glEnable( GL_TEXTURE_2D );
    
    gfx->SetViewport(0, 0, texsize, texsize);
    Math::Mat4 prjmat = Math::Mat4::Ortho(0, texsize, texsize, 0, 1.0f, -1.0f);
    gfx->SetProjection(&prjmat);

    //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER,0.1f);
    
    
    // generate font
    unsigned char temp_bitmap[texsize*texsize];
    gen_bitmapdata(temp_bitmap, texsize, fontsize, blocksize, argv[4]);
    GLuint ftex;
    glGenTextures(1, &ftex);
    glBindTexture(GL_TEXTURE_2D, ftex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, texsize,texsize, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

/*

     float x = blocksize / 2.0f;
     float y = blocksize / 2.0f;
     float posstep = blocksize;
     //char *text = "hello world";
     qb->Reset();
     qb->Begin();
		   // assume orthographic projection with units = screen pixels, origin at top left
      glBindTexture(GL_TEXTURE_2D, ftex);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      //while (*text) {
      for(char i = char_range[0]; i < char_range[1]; ++i)
      {
            stbtt_aligned_quad q;
            float tx = 0, ty = 0;
            stbtt_GetBakedQuad(cdata, texsize,texsize, i, &tx,&ty,&q,1);
            float w,h;
            w = q.x1 - q.x0;
            h = q.y1 - q.y0;
            
            qb->SetTextureSubset(q.s0,q.t0,q.s1,q.t1);
            qb->AddCentered((int)x,(int)y,(int)w,(int)h);
                            
            x += posstep;
            if (x > texsize)
            {
              x = blocksize / 2.0f;
              y += posstep;
            }
      }
      qb->End();
    */

    while(win->IsOpen())
    {
      /*gfx->BindTexture(0);
		  gfx->BindFrameBufferObject(pFBO);
		  glClear(GL_COLOR_BUFFER_BIT);
		  
      qb->Draw();
  		gfx->UnbindFrameBufferObject();*/
		
      glClear(GL_COLOR_BUFFER_BIT);
  		//gfx->BindTexture(tex0);
  		glBindTexture(GL_TEXTURE_2D, ftex);
  		outFBO->Draw();
  		//glBindTexture(GL_TEXTURE_2D, ftex);
  		//qb->Draw();
      win->Swap();
    }
    
    delete outFBO;
    

    //res->Release(img);
    delete kernel;
    return 0;
}

