#ifndef _PXF_GUI_H
#define _PXF_GUI_H_

#include <Pxf/Kernel.h>
#include <Pxf/Input/InputDefs.h>

namespace Pxf {
	namespace Graphics {
	namespace Rect
	{ class Rect_t; }}

	class Gui
	{
	public:
		struct Widget_t
		{

		};

		struct State_t
		{
			// Screen
			float screen_res_w;
			float screen_res_h;

			// Gui items
			void* hot_item;
			void* next_hot_item;
			void* active_item;
			void* last_active_item;
			void* focused_item;

			// Input (Mouse)
			float mouse_x;
			float mouse_y;
			float mouse_dx;
			float mouse_dy;

			unsigned int mouse_button_state;
			unsigned int mouse_button_state_prev;

			// Input (Keyboard)
			int last_key;
			char last_char;
		};

	protected:
		State_t m_State;
		Pxf::Kernel* m_Kernel;
		Pxf::Input::InputDevice* m_Input;

		int m_mouse_x_old, m_mouse_y_old;

		/* function pointer typedef for rendering callbacks */
		typedef void (*draw_button_func)(void* id, const Graphics::Rect::Rect_t *rect, void* _font, const char* text, bool has_focus, void* extra);
	public:
		Gui(Pxf::Kernel* _Kernel)
			: m_Kernel(_Kernel)
			, m_mouse_x_old(0)
			, m_mouse_y_old(0)
		{
			m_Input = m_Kernel->GetInputDevice();
		}

		enum InputAction
		{
			// if do_button() & INPUT_ACTION: do_action()
			INPUT_NOACTION	= 0,
			INPUT_ACTION	= 1,
			INPUT_LMB		= INPUT_ACTION | 2,
			INPUT_MMB		= INPUT_ACTION | 4,
			INPUT_RMB		= INPUT_ACTION | 8,
			INPUT_KBD_RET   = INPUT_ACTION | 16,
		};

		// State utilities
		bool MouseInside(Graphics::Rect::Rect_t* rect);
		bool MouseDown(int button); /* mouse is down */
		bool MouseClicked(int button); /* mouse was JUST clicked */
		bool MouseReleased(int button); /* mouse was JUST released */

		void SetResolution(float w, float h)
		{
			m_State.screen_res_w = w;
			m_State.screen_res_h = h;
		}

		void SetHotItem(void* id)
		{
			m_State.hot_item = id;
		}

		void SetActiveItem(void* id)
		{
			if (m_State.active_item)
				m_State.last_active_item = m_State.active_item;
			m_State.active_item = id;
		}

		void SetFocusedItem(void* id)
		{
			m_State.last_key = 0;
			m_State.last_char = 0;
			m_State.focused_item = id;
		}

		float GetMouseX(){return m_State.mouse_x;}
		float GetMouseY(){return m_State.mouse_y;}

		void* GetHotItem(){return m_State.hot_item;}
		void* GetActiveItem(){return m_State.active_item;}
		void* GetFocusedItem(){return m_State.focused_item;}
	
		// Gui Logic
		bool Initialize();
		void Update();

		InputAction DoButton(void* id, Graphics::Rect::Rect_t* rect, const char* text, draw_button_func draw_func, void* extra);
	};
}

#endif // _PXF_GUI_H_