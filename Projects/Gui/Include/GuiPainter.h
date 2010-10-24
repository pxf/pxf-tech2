#ifndef _PXF_GRAPHICS_GUIPAINTER_H_
#define _PXF_GRAPHICS_GUIPAINTER_H_

namespace Pxf {
namespace Graphics {
	namespace Rect { struct Rect_t; }
	class Font;
	void DrawButton(void* id, const Rect::Rect_t *rect, Font* _Font, const char* text, bool has_focus, void* extra = 0);
	void DrawTextfield(void* id, const Rect::Rect_t* rect, Font* _Font, const char* text, int cursor_pos, bool has_focus, void* extra = 0);
	void DrawHorizontalSlider(void* id, const Rect::Rect_t *rect, const Rect::Rect_t *grabber, Font* _Font, bool has_focus, void* extra = 0);
}
}

#endif //_PXF_GRAPHICS_GUIPAINTER_H_