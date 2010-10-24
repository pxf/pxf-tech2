#include <Pxf/Graphics/Rect.h>

using namespace Pxf;
using namespace Pxf::Graphics;

void Rect::RectHSplitT(const Rect::Rect_t* rect, Rect::Rect_t* top, Rect::Rect_t* bottom, float where)
{
	top->x = rect->x;
	top->y = rect->y;
	top->w = rect->w;
	top->h = where;
	bottom->x = rect->x;
	bottom->y = rect->y + where;
	bottom->w = rect->w;
	bottom->h = rect->h - where;
}

void Rect::RectHSplitB(const Rect::Rect_t* rect, Rect::Rect_t* top, Rect::Rect_t* bottom, float where)
{
	top->x = rect->x;
	top->y = rect->y;
	top->w = rect->w;
	top->h = rect->h-where;
	bottom->x = rect->x;
	bottom->y = rect->y + rect->h - where;
	bottom->w = rect->w;
	bottom->h = where;
}

void Rect::RectHSplitM(const Rect::Rect_t* rect, Rect::Rect_t* top, Rect::Rect_t* bottom)
{
	float half_height = rect->h/2.f;
	RectHSplitT(rect, top, bottom, half_height);
}

void Rect::RectVSplitL(const Rect::Rect_t* rect, Rect::Rect_t* left, Rect::Rect_t* right, float where)
{
	left->x = rect->x;
	left->y = rect->y;
	left->w = where;
	left->h = rect->h;
	right->x = rect->x + where;
	right->y = rect->y;
	right->w = rect->w - where;
	right->h = rect->h;
}

void Rect::RectVSplitR(const Rect::Rect_t* rect, Rect::Rect_t* left, Rect::Rect_t* right, float where)
{
	left->x = rect->x;
	left->y = rect->y;
	left->w = rect->w-where;
	left->h = rect->h;
	right->x = rect->x + rect->w - where;
	right->y = rect->y;
	right->w = where;
	right->h = rect->h;
}

void Rect::RectVSplitM(const Rect::Rect_t* rect, Rect::Rect_t* left, Rect::Rect_t* right)
{
	float half_width = rect->w/2.f;
	RectVSplitL(rect, left, right, half_width);
}

void Rect::RectMarginH(const Rect::Rect_t* rect, Rect::Rect_t* new_rect, float margin)
{
	new_rect->y = rect->y-margin;
	new_rect->h = rect->h+2*margin;
}

void Rect::RectMarginV(const Rect::Rect_t* rect, Rect::Rect_t* new_rect, float margin)
{
	new_rect->x = rect->x-margin;
	new_rect->w = rect->w+2*margin;
}

void Rect::RectMargin(const Rect::Rect_t* rect, Rect::Rect_t* new_rect, float margin)
{
	RectMarginH(rect, new_rect, margin);
	RectMarginV(rect, new_rect, margin);
}

void Rect::RectCenterH(const Rect_t* rect, Rect_t* rect1)
{
	float margin_h = rect->h - rect1->h;
	rect1->y = rect->y + margin_h/2;
}

void Rect::RectCenterV(const Rect_t* rect, Rect_t* rect1)
{
	float margin_w = rect->w - rect1->w;
	rect1->x = rect->x + margin_w/2;
}

void Rect::RectCenter(const Rect_t* rect, Rect_t* rect1)
{
	RectCenterH(rect, rect1);
	RectCenterV(rect, rect1);
}
