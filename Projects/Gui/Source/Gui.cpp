#include <Gui.h>
#include <GuiPainter.h>
#include <Pxf/Input/InputDevice.h>
#include <Pxf/Graphics/Rect.h>
#include <Pxf/Graphics/Font.h>
#include <Pxf/Graphics/PrimitiveBatch.h>

#include <Pxf/Resource/ResourceManager.h>
#include <Pxf/Resource/Font.h>

#include <cstdio>

using namespace Pxf;
using namespace Pxf::Graphics;

/*
	State utilities
*/

bool Gui::MouseInside(Rect::Rect_t* rect)
{
	if (m_State.mouse_x < rect->x ||
		m_State.mouse_y < rect->y ||
		m_State.mouse_x >= rect->x+rect->w || 
		m_State.mouse_y >= rect->y+rect->h)
		return false;
	return true;
}

static int translate_button(int button)
{
	if (button >= Input::MOUSE_1)
		button = button - Input::MOUSE_1;
	else
		button--;
	return button;
}

bool Gui::MouseDown(int button)
{
	button = translate_button(button);
	return (m_State.mouse_button_state>>button) & 1;
}

bool Gui::MouseClicked(int button)
{
	button = translate_button(button);
	bool last_down = (m_State.mouse_button_state_prev>>button)&1;
	bool curr_down = (m_State.mouse_button_state>>button)&1;
	return !last_down && curr_down;
}

bool Gui::MouseReleased(int button)
{
	button = translate_button(button);
	bool last_down = (m_State.mouse_button_state_prev>>button)&1;
	bool curr_down = (m_State.mouse_button_state>>button)&1;
	return last_down && !curr_down;
}

/* GUI Logic */

Gui::Gui(Pxf::Kernel* _Kernel)
	: m_Kernel(_Kernel)
	, m_mouse_x_old(0)
	, m_mouse_y_old(0)
	, m_Font(0)
	, m_pb(0)
{
	m_Input = m_Kernel->GetInputDevice();
	m_pb = new PrimitiveBatch(m_Kernel->GetGraphicsDevice());
	Resource::Font* bitmapfont = m_Kernel->GetResourceManager()->Acquire<Resource::Font>("data/Proggy.pfnt");
	m_Font = new Graphics::Font(m_Kernel->GetGraphicsDevice(), bitmapfont);
	m_Font->SetTextureFilters(TEX_FILTER_NEAREST, TEX_FILTER_NEAREST);

	Initialize();
}

bool Gui::Initialize()
{
	// return false if some gui-resource fails to load (textures, fonts)
	m_Input->SetMouseMode(Input::MODE_ABSOLUTE);

	m_State.hot_item = 0;
	m_State.next_hot_item = 0;
	m_State.active_item = 0;
	m_State.last_active_item = 0;
	m_State.focused_item = 0;
	return true;
}

void Gui::Update()
{

	int mouse_x, mouse_y;
	m_Input->GetMousePos(&mouse_x, &mouse_y);
	int mouse_dx = mouse_x - m_mouse_x_old, mouse_dy = mouse_y - m_mouse_y_old;
	m_mouse_x_old = mouse_x; m_mouse_y_old = mouse_y;
	unsigned mouse_buttons = m_Input->GetButtonStates();
	
	/* set hot item */
	m_State.hot_item = m_State.next_hot_item;
	m_State.next_hot_item = 0;

	if (m_State.active_item)
		m_State.hot_item = m_State.active_item;

	// input
	if (m_State.focused_item != 0 && m_Input->GetLastKey())
	{
		int k = m_Input->GetLastKey();
		int c = m_Input->GetLastChar();
		m_Input->ClearLastKey();
		m_Input->ClearLastChar();

		// Only ISO 8859-1
		if (c < 256)
		{
			m_State.last_key = k;
			m_State.last_char = static_cast<char>(c);
		}
	}
	// Discard characters from previous frame
	else
	{
		m_State.last_key = 0;
		m_State.last_char = 0;
	}

	m_State.mouse_x = (float)mouse_x;
	m_State.mouse_y = (float)mouse_y;
	m_State.mouse_dx = (float)mouse_dx;
	m_State.mouse_dy = (float)mouse_dy;
	m_State.mouse_button_state_prev = m_State.mouse_button_state;
	m_State.mouse_button_state = mouse_buttons;
}


Gui::InputAction Gui::DoButton(void* id, Rect::Rect_t* rect, const char* text, draw_button_func draw_func, void* extra)
{
	InputAction ret = INPUT_NOACTION;
	bool has_focus = GetFocusedItem() == id;
	bool inside = MouseInside(rect);

	if (m_State.focused_item == 0)
		m_State.focused_item = id;
	else if(m_State.focused_item == id && m_State.last_key == Input::TAB)
		m_State.focused_item = 0;

	if (inside)
		SetHotItem(id);

	if (has_focus && m_State.last_key == Input::ENTER)
	{
		ret = INPUT_KBD_RET;
		m_State.last_key = 0;
		m_State.last_char = 0;
		
		/* This bugs if the mouse is hoovering the button while
		   the enter key is pressed. It sort-of interfer with the method below.*/
		//SetActiveItem(id);
	}
	else if (GetActiveItem() == id && !MouseDown(Input::MOUSE_1))
	{
		if (inside)
			ret = INPUT_LMB;
		else
			m_State.hot_item = 0;
		SetActiveItem(0);
	}
	else if (GetHotItem() == id && GetActiveItem() == 0 && MouseDown(Input::MOUSE_1))
	{
		SetFocusedItem(id);
		SetActiveItem(id);
	}

	if (draw_func)
		draw_func(id, rect, m_Font, text, has_focus, extra);
	else
		DrawButton(m_Kernel->GetGraphicsDevice(), this, m_pb, id, rect, m_Font, text, has_focus, extra);
	return ret;
}
