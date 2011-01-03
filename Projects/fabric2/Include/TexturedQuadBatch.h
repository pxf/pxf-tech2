#ifndef __FABRIC_TEXTUREDQUADBATCH_H__
#define __FABRIC_TEXTUREDQUADBATCH_H__

#include "QuadBatch.h"

namespace Fabric
{
    class TexturedQuadBatch : public QuadBatch
    {
    public:
        TexturedQuadBatch(unsigned int _size, const char* _texture_filepath, float* _currentdepth, Pxf::Math::Vec4f* _currentcolor, Pxf::Math::Mat4* _transformmatrix = NULL, bool _linear = false);
				TexturedQuadBatch(unsigned int _size, int _width, int _height, int _channels, const unsigned char* _texture_data, float* _currentdepth, Pxf::Math::Vec4f* _currentcolor, Pxf::Math::Mat4* _transformmatrix = NULL, bool _linear = false);
        virtual ~TexturedQuadBatch();
        
        void SetTextureSubset(float tl_u, float tl_v, float br_u, float br_v);
        void Reset();
        void Begin();
        void End();
		void Draw();

    private:
        const char* m_TextureFilepath;
        Pxf::Graphics::Texture* m_Texture;
        
        
        
    };
} /* Fabric */

#endif /* __FABRIC_TEXTUREDQUADBATCH_H__ */
