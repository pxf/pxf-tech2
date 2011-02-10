#ifndef _DERPRENDERER_SIMPLEQUAD_H_
#define _DERPRENDERER_SIMPLEQUAD_H_

#include <Pxf/Math/Vector.h>

#include <Pxf/Graphics/VertexBuffer.h>

namespace Derp
{
	class SimpleQuad
	{
	public:
		struct QuadVertex
	  {
	      Pxf::Math::Vec3f position;
	      Pxf::Math::Vec4f color;
	      Pxf::Math::Vec2f coord;
	  };
		
		SimpleQuad (float x1, float y1, float x2, float y2);
		virtual ~SimpleQuad ();
		
		void Draw();

	private:
		Pxf::Graphics::VertexBuffer *m_QuadVB;
	};
} /* Derp */

#endif /* _DERPRENDERER_SIMPLEQUAD_H_ */