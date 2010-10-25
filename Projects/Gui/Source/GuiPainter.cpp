#include <Pxf/Math/Math.h>
#include <Pxf/Graphics/GraphicsDevice.h>
#include <Pxf/Graphics/Font.h>
#include <Pxf/Graphics/PrimitiveBatch.h>
#include <Pxf/Graphics/Rect.h>
#include <Gui.h>
#include <GuiPainter.h>

using namespace Pxf;
using namespace Pxf::Math;

#define B2F(r, g, b) (r)/255.f, (g)/255.f, (b)/255.f, 1.f

static Vec4f ColorBorder(B2F(185, 184, 170));
static Vec4f ColorBase(B2F(238, 237, 224));
static Vec4f ColorFocusBorder(B2F(178, 195, 219));

static Vec4f ColorButtonHighlight(B2F(255, 255, 255));

static Vec4f ColorSliderBase(B2F(255, 250, 242));
static Vec4f ColorSliderGrabber(B2F(238, 237, 224));
static Vec4f ColorSliderGrabberHighlight(B2F(223, 222, 204));

#undef B2F

void Graphics::DrawButton(GraphicsDevice* gfx, Gui* gui, PrimitiveBatch* pb, void* id, const Rect::Rect_t *rect, Font* _Font, const char* text, bool has_focus, void* extra)
{
	(extra);

	if (!_Font)
		return;

	float text_height;
	float text_width;
	_Font->MeasureString(text, &text_width, &text_height);

	float text_x = rect->x + (rect->w/2) - (text_width/2);
	float text_y = rect->y + (rect->h/2) - (text_height/2);

	pb->QuadsBegin();

	// Border
	pb->SetColor(&ColorBorder);
	pb->QuadsDrawTopLeft(rect->x, rect->y, rect->w, rect->h);
	
	// Draw highlight if hot (not if active though)
	if (gui->GetHotItem() == id && gui->GetActiveItem() != id)
		pb->SetColor(&ColorButtonHighlight);
	else
		pb->SetColor(&ColorBase);
	pb->QuadsDrawTopLeft(rect->x+1, rect->y+1, rect->w-2.f, rect->h-2.f);

	// Fill button with base color
	pb->SetColor(&ColorBase);
	pb->QuadsDrawTopLeft(rect->x+2.f, rect->y+2.f, rect->w-3.f, rect->h-3.f);
	pb->QuadsEnd();

	// FIXME: This is expensive! LineStripple eats up ~9 FPS
	// Dotted focus border
	if (has_focus)
	{
		//glLineStipple(1, 0xAAAA);
		//glEnable(GL_LINE_STIPPLE);
		pb->LinesBegin();
		pb->SetColor(&ColorFocusBorder);
		pb->LinesDrawFrame(rect->x+3, rect->y+3, rect->w-5, rect->h-5);
		pb->LinesEnd();
		//glDisable(GL_LINE_STIPPLE);
	}

	gfx->Print(_Font, text_x, text_y, 1.f, text);
}


void Graphics::DrawTextfield(GraphicsDevice* gfx, Gui* gui, PrimitiveBatch* pb, void* id, const Rect::Rect_t* rect, Font* _Font, const char* text, int cursor_pos, bool has_focus, void* extra)
{
	/*
	// Should only draw text that is visible inside the Rect_t.
	// Should use the cursor_pos to scroll the text when moving around.
	(id);
	(extra);
	PrimitiveBatch::QuadsBegin();
	PrimitiveBatch::SetColor(.4f, .4f, .4f, .6f);
	PrimitiveBatch::QuadsDrawTopLeft(rect->x, rect->y, rect->w, rect->h);
	PrimitiveBatch::SetColor(.8f, .8f, .8f, .6f);
	PrimitiveBatch::QuadsDrawTopLeft(rect->x+1, rect->y+1, rect->w-2, rect->h-2);
	PrimitiveBatch::QuadsEnd();
	Graphics::Print(_Font, rect->x+1, rect->y+1, 1.f, text);

	float cw;
	_Font->MeasureString(text, &cw, 0, cursor_pos);

	PrimitiveBatch::LinesBegin();
	if (has_focus)
	{
		PrimitiveBatch::SetColor(0.2f, 0.2f, 0.2f, 1.f);
		PrimitiveBatch::LinesDraw(rect->x+cw, rect->y, rect->x+cw, rect->y+rect->h);
	}
	PrimitiveBatch::LinesEnd();
	*/
}

void Graphics::DrawHorizontalSlider(GraphicsDevice* gfx, Gui* gui, PrimitiveBatch* pb, void* id, const Rect::Rect_t *rect, const Rect::Rect_t *grabber, Font* _Font, bool has_focus, void* extra)
{
	/*
	(extra);
	(_Font);
	PrimitiveBatch::QuadsBegin();

	// Slider border
	PrimitiveBatch::SetColor(&ColorBorder);
	PrimitiveBatch::QuadsDrawTopLeft(rect->x, rect->y, rect->w, rect->h);

	// Slider fill
	PrimitiveBatch::SetColor(&ColorSliderBase);
	PrimitiveBatch::QuadsDrawTopLeft(rect->x+1, rect->y+1, rect->w-2, rect->h-2);

	// Grabber border
	PrimitiveBatch::SetColor(&ColorBorder);
	PrimitiveBatch::QuadsDrawTopLeft(grabber->x, grabber->y, grabber->w, grabber->h);

	if (gui->GetActiveItem() == id)
		PrimitiveBatch::SetColor(&ColorSliderGrabberHighlight);
	else
		PrimitiveBatch::SetColor(&ColorSliderGrabber);
	PrimitiveBatch::QuadsDrawTopLeft(grabber->x+1, grabber->y+1, grabber->w-2, grabber->h-2);
	PrimitiveBatch::QuadsEnd();

	//glLineStipple(1, 0xAAAA);
	//glEnable(GL_LINE_STIPPLE);
	PrimitiveBatch::LinesBegin();
	if (has_focus)
	{
		PrimitiveBatch::SetColor(&ColorFocusBorder);
		PrimitiveBatch::LinesDrawFrame(grabber->x+2, grabber->y+2, grabber->w-5, grabber->h-5);

	}
	PrimitiveBatch::LinesEnd();
	//glDisable(GL_LINE_STIPPLE);
	*/
}