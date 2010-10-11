--[[
  Special window handling for the GUI.
]]

gui.windows = {windowlist = {}}

function gui.windows:add(wnd)
  -- add window to top of list
  table.insert(self.windowlist, 1, wnd)
  
  -- add to widget root
  gui.widgets:addwidget(wnd)
end

function gui.windows:remove(wnd)
  -- remove from window list
  table.remove(self.windowlist, wnd)
end

function gui.windows:setactive(wnd)
  -- remove from window list
  table.remove(self.windowlist, wnd)
  -- ... and re-insert it at the top
  table.insert(self.windowlist, 1, wnd)
  
  -- do the same for root widget list
  table.remove(gui.widgets.childwidgets, wnd)
  table.insert(widgets.childwidgets, wnd)
end

---------------------------------------
-- standard window gui widget
function gui:create_window(x,y,w,h,modal,label)
	local window = gui:create_basewidget(x,y,w,h)
	local window_label = gui:create_labelpanel(0,0,w,20, label)
	local window_panel = gui:create_basewidget(0,20,w,h-20)
	window.panel = window_panel
	window.label = window_label
	
	--local minimize_button = gui:create_staticpanel(w-40,0,20,20)
	--local close_button = gui:create_staticpanel(w-20,0,20,20)
	--local window_label = gui:create_labelpanel(6,6,0,0,label)
	--local minimize_label_arrow = gui:create_labelpanel(0,0,0,0,">")
	--local close_label_icon = gui:create_labelpanel(6,5,0,0,"x")

	--[[function base_window:destroy_self()
		self = nil
	end

	minimize_label_arrow.super_draw = minimize_label_arrow.draw
	close_label_icon.super_draw = close_label_icon.draw

	local top_container = gui:create_basewidget(0,0,w,20)

	close_button:addwidget(close_label_icon)
	minimize_button:addwidget(minimize_label_arrow)

	top_container:addwidget(window_label)
	top_container:addwidget(close_button)
	top_container:addwidget(minimize_button)
	

	local window_state = { maximized = 0, minimized = 1 }

	base_window.state = window_state.maximized
	base_window.height = h
	base_window:addwidget(top_container)

	base_window.superdraw = base_window.draw
	
	function minimize_label_arrow:draw()
		local move_offset = {x = 0, y = 0}
		local move_dir = 0

		if (base_window.state == window_state.maximized) then
			--gfx.rotate(math.pi * 0.5)
			gfx.translate(move_offset.x + 1,move_offset.y)
			minimize_label_arrow:super_draw()
			--gfx.rotate(-math.pi * 0.5)
			gfx.translate(- (move_offset.x + 1),-move_offset.y)
		end
		
		if (base_window.state == window_state.minimized) then
			--gfx.rotate(-math.pi * 0.5)
			gfx.translate(-move_offset.x + 1,-move_offset.y)
			minimize_label_arrow:super_draw()
			--gfx.rotate(math.pi * 0.5)
			gfx.translate(move_offset.x - 1,move_offset.y)
		end
	end

	function base_window:minimize()
	  self:needsredraw()
		self.drawbox.h = 20
		self.hitbox.h = 20
		self.state = window_state.minimized
		self:needsredraw()
		--snd.stopsound(balls_id)
	end

	function base_window:maximize()
	  self:needsredraw()
		self.drawbox.h = self.height
		self.drawbox.h = self.height
		self.state = window_state.maximized
		self:needsredraw()
		--snd.playsound(balls_id,true)
	end

	function minimize_button:mouserelease(mx,my,button)
		if (button == inp.MOUSE_LEFT) then
			if (base_window.state == window_state.minimized) then
				base_window:maximize()
			else
				base_window:minimize()
			end
		end
	end

	function close_button:mouserelease(mx,my,button)
		if (button == inp.MOUSE_LEFT) then
			base_window:destroy()
		end
	end]]
	
	function window_panel:draw(force)
	  if (self.redraw_needed or force) then
  		gfx.translate(self.drawbox.x,self.drawbox.y)
  		
  		gfx.drawtopleft(0, 0, self.drawbox.w, self.drawbox.h)
	
  		for k,v in pairs(self.childwidgets) do
  			v:draw(force)
  		end

  		gfx.translate(-self.drawbox.x,-self.drawbox.y)
  	end
	end

	function window:draw(force)
	  if (self.redraw_needed or force) then
	    local oldtex = gfx.bindtexture(0)
  		gfx.translate(self.drawbox.x,self.drawbox.y)
  		
  		local r,g,b = gfx.getcolor()
  		gfx.setcolor(1,0,0)
  		gfx.drawtopleft(0, 0, self.drawbox.w, self.drawbox.h)
  		gfx.setcolor(r,g,b)
	
  		for k,v in pairs(self.childwidgets) do
  			v:draw(force)
  		end

  		gfx.translate(-self.drawbox.x,-self.drawbox.y)
  		gfx.bindtexture(oldtex)
  	end
	end
	
	-- add title/lable to window widget
	window:addwidget(window_label)
	
	-- add panel to window widget
	window:addwidget(window_panel)

	return window
end
