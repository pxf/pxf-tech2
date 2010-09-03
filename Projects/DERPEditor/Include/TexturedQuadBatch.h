#ifndef __DERPEDITOR_TEXTUREDQUADBATCH_H__
#define __DERPEDITOR_TEXTUREDQUADBATCH_H__

#include "QuadBatch.h"

namespace DERPEditor
{
    class TexturedQuadBatch : public QuadBatch
    {
    public:
        TexturedQuadBatch(unsigned int _size, Pxf::Math::Mat4* _transformmatrix, const char* _texture_filepath);
        ~TexturedQuadBatch();
        
        void SetTextureSubset(float tl_u, float tl_v, float br_u, float br_v);
        void Reset();
        void Begin();
        void End();
		void Draw();

    private:
        const char* m_TextureFilepath;
        Pxf::Graphics::Texture* m_Texture;
        
        
        
    };
} /* DERPEditor */

#endif /* __DERPEDITOR_TEXTUREDQUADBATCH_H__ */
