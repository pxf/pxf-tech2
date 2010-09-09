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
            Pxf::Math::Vec4f color;
            Pxf::Math::Vec2f coord;
        };
        
        QuadBatch(unsigned int _size, float* _currentdepth, Pxf::Math::Vec4f* _currentcolor, Pxf::Math::Mat4* _transformmatrix = NULL);
        virtual ~QuadBatch();
        
        //void SetColor(float r, float g, float b) { m_CurrentColor = Pxf::Math::Vec4f(r, g, b, m_CurrentColor.a); };
        //void SetAlpha(float a) { m_CurrentColor = Pxf::Math::Vec4f(m_CurrentColor.r, m_CurrentColor.g, m_CurrentColor.b, a); };
		    virtual void SetTextureSubset(float tl_u, float tl_v, float br_u, float br_v);
		    void ResetTextureSubset();
		/*void SetRotation(float angle); // Rotate following quad around its own axis
		void Rotate(float angle); // Rotate coord system
        void Translate(float x, float y); // Translate coord system
        void LoadIdentity(); // Reset coord system
        */
        //void SetDepth(float d) { m_CurrentDepth = d; };
		
		virtual void Reset();
        virtual void Begin();
		void AddFreeform(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3);
		void AddTopLeft(float x, float y, float w, float h);
		void AddCentered(float x, float y, float w, float h);
        virtual void End();
		virtual void Draw();
		
	private:
        Pxf::Graphics::VertexBuffer *m_VertexBuffer;
        QuadVertex* m_pVertBuf;
        unsigned int m_VertBufSize;
        
        int m_CurrentVert;
        
        // transformation
        Pxf::Math::Mat4* m_Transformation;
        
        // per quad states
        float* m_CurrentDepth;
        Pxf::Math::Vec2f m_CurrentCoords[4];
        Pxf::Math::Vec4f* m_CurrentColor;
    };
} /* DERPEditor */

#endif /* __DERPEDITOR_QUADBATCH_H__ */
