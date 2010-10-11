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
  local i = nil
  for k,v in pairs(self.windowlist) do
    if v == wnd then
      i = k
      break
    end
  end
  table.remove(self.windowlist, k)
end

function gui.windows:setactive(wnd)
  -- if not the top window is modal, then change active window
  if not (self.windowlist[1].modal) then
    -- remove from window list
    local i = nil
    for k,v in pairs(self.windowlist) do
      if v == wnd then
        i = k
        break
      end
    end
    table.remove(self.windowlist, k)
    -- ... and re-insert it at the top
    table.insert(self.windowlist, 1, wnd)
  
    -- do the same for root widget list
    for k,v in pairs(gui.widgets.childwidgets) do
      if v == wnd then
        i = k
        break
      end
    end
    table.remove(gui.widgets.childwidgets, i)
    table.insert(gui.widgets.childwidgets, wnd)
  end
end

---------------------------------------
-- standard window gui widget
function gui:create_window(x,y,w,h,modal,label)
	local window = gui:create_basewidget(x,y,w,h)
	local window_label = gui:create_labelpanel(0,0,w-40,20, label)
	local window_panel = gui:create_basewidget(0,20,w,h-20)
	local window_closebutton = gui:create_simplebutton(w-20,0,20,20,"x",function(self) self.parent:destroy() end)
	local window_sizebutton = gui:create_simplebutton(w-40,0,20,20,"v",function() print("lol min/max") end)
	window.panel = window_panel
	window.label = window_label
	
	-- if the window is in modal mode ( = this window needs to close before we continue)
	window.modal = modal
	
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
	
	-- hackety-hack to get all child widgets to be able to move the window
	function window:mousedrag(dx,dy,button)
		if (button == inp.MOUSE_LEFT) then
			window:needsredraw()
			window.drawbox.x = window.drawbox.x + dx
			window.drawbox.y = window.drawbox.y + dy
			window.hitbox.x = window.hitbox.x + dx
			window.hitbox.y = window.hitbox.y + dy
			window:needsredraw()
		end
	end
	window_panel.mousedrag = window.mousedrag
	window_label.mousedrag = window.mousedrag
	
	-- set current window active if window gets active
	function window:mousepush(mx,my,button)
    gui.windows:setactive(window)
  end
  window_panel.mousepush = window.mousepush
	window_label.mousepush = window.mousepush
	
	-- change so that destroy also removes the window from the window list
	window.superdestroy = window.destroy
	function window:destroy()
	  self:superdestroy()
	  gui.windows:remove(self)
  end
	
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
	
	-- add top buttons to window
	window:addwidget(window_closebutton)
	window:addwidget(window_sizebutton)

	return window
end
