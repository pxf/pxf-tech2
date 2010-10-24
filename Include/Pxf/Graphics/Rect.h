#ifndef __PXF_GRAPHICS_RECT_H__
#define __PXF_GRAPHICS_RECT_H__

// TODO: Integrate more with stuff in Graphics/*

namespace Pxf
{
	namespace Graphics{
		namespace Rect
		{
			struct Rect_t
			{
				float x, y, w, h;
			};

			// Splitting
			void RectHSplitT(const Rect_t* rect, Rect_t* top, Rect_t* bottom, float where);
			void RectHSplitB(const Rect_t* rect, Rect_t* top, Rect_t* bottom, float where);
			void RectHSplitM(const Rect_t* rect, Rect_t* top, Rect_t* bottom);
			void RectVSplitL(const Rect_t* rect, Rect_t* left, Rect_t* right, float where);
			void RectVSplitR(const Rect_t* rect, Rect_t* left, Rect_t* right, float where);
			void RectVSplitM(const Rect_t* rect, Rect_t* left, Rect_t* right);

			// Margin
			void RectMarginH(const Rect_t* rect, Rect_t* new_rect, float margin);
			void RectMarginV(const Rect_t* rect, Rect_t* new_rect, float margin);
			void RectMargin (const Rect_t* rect, Rect_t* new_rect, float margin);

			// Center rect1 inside rect
			void RectCenterH(const Rect_t* outer_rect, Rect_t* inner_rect);
			void RectCenterV(const Rect_t* outer_rect, Rect_t* inner_rect);
			void RectCenter (const Rect_t* outer_rect, Rect_t* inner_rect);
		}	
	}
}

#endif // __PXF_GRAPHICS_RECT_H__
