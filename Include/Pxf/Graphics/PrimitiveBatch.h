#ifndef _PXF_GRAPHICS_PRIMITIVEBATCH_H_
#define _PXF_GRAPHICS_PRIMITIVEBATCH_H_

#include <Pxf/Graphics/DeviceResource.h>
#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Math/Vector.h>

/*
http://www.opengl.org/resources/faq/technical/transformations.htm
9.030 How do I draw 2D controls over my 3D rendering?
glMatrixMode (GL_MODELVIEW); glLoadIdentity (); glTranslatef (0.375, 0.375, 0.);
*/

namespace Pxf {
namespace Graphics {
	class VertexBuffer;
	class PrimitiveBatch : public DeviceResource
	{
	protected:
		VertexBuffer* m_VertexBuffer;
	public:
		PrimitiveBatch(GraphicsDevice* _pDevice);

		virtual ~PrimitiveBatch();

		void SetColor(float r, float g, float b, float a);
		void SetColor(Math::Vec4f* c);

		void PointsBegin();
		void PointsDraw(float x, float y);
		void PointsEnd();

		void LinesBegin();
		void LinesDraw(float x0, float y0, float x1, float y1);
		void LinesDrawFrame(float x, float y, float w, float h);
		void LinesEnd();

		void QuadsBegin();
		void QuadsDrawCentered(float x, float y, float w, float h);
		void QuadsDrawTopLeft(float x, float y, float w, float h);
		void QuadsDrawFreeform(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3);
		void QuadsSetRotation(float angle);
		void QuadsSetTextureSubset(float tl_u, float tl_v, float br_u, float br_v);
		void QuadsEnd();

	}; // PrimitiveBatch
} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_PRIMITIVEBATCH_H_
