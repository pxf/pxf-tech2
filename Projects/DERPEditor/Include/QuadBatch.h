#ifndef __DERPEDITOR_QUADBATCH_H__
#define __DERPEDITOR_QUADBATCH_H__

#include <Pxf/Math/Vector.h>
#include <Pxf/Graphics/VertexBuffer.h>

namespace DERPEditor
{
    class QuadBatch
    {
    public:
        
        struct QuadVertex
        {
            Pxf::Math::Vec3f position;
            Pxf::Math::Vec2f coord;
            Pxf::Math::Vec4f color;
        };
        
        QuadBatch(int _size);
        ~QuadBatch();
        
        /*void SetColor(float r, float g, float b);
        void SetAlpha(float a);
		void SetTextureSubset(float tl_u, float tl_v, float br_u, float br_v);
		void SetRotation(float angle); // Rotate following quad around its own axis
		void Rotate(float angle); // Rotate coord system
        void Translate(float x, float y); // Translate coord system
        void LoadIdentity(); // Reset coord system
        */
        void SetDepth(float d) { m_CurrentDepth = d; };
		
		void Reset();
		/*void AddFreeform(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3);
		void AddTopLeft(float x, float y, float w, float h);
		*/
		void AddCentered(float x, float y, float w, float h);
		void Draw();
		
	private:
        Pxf::Graphics::VertexBuffer *m_VertexBuffer;
        int m_VertBufSize;
        
        int m_CurrentVert;
        
        float m_CurrentDepth;
        Pxf::Math::Vec2f m_CurrentCoords[4];
        Pxf::Math::Vec4f m_CurrentColor;
    };
} /* DERPEditor */

#endif /* __DERPEDITOR_QUADBATCH_H__ */
