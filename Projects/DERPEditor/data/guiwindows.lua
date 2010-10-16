--[[
  Special window handling for the GUI.
]]

gui.windows = {windowlist = {}}

function gui.windows:add(wnd)
  -- see if window is already open
  local was_active = false
  for k,v in pairs(gui.windows.windowlist) do
  --for i=1,#gui.windows.windowlist do
    --local v = gui.windows.windowlist[i]
    if v and (v.winid == wnd.winid) then
      
      was_active = true
      -- already open!
      -- set focus instead
      gui.windows:setactive(v)
      
      break
    end
  end
  
  if not was_active then
    -- add window to top of list
    table.insert(self.windowlist, 1, wnd)
  
    -- if window is modal, the add the dimmer widget first
    if (wnd.modal) then
      gui.widgets:addwidget(wnd.dimmer)
    end
  
    -- add to widget root
    gui.widgets:addwidget(wnd)
  end
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
  --table.remove(self.windowlist, k)
  self.windowlist[i] = nil
end

function gui.windows:setactive(wnd)
  -- remove from window list
  local i = nil
  --[[for k,v in pairs(self.windowlist) do
    if v == wnd then
      i = k
      break
    end
  end
  table.remove(self.windowlist, k)]]
  --gui.windows:remove(wnd)
  -- ... and re-insert it at the top
  --table.insert(self.windowlist, 1, wnd)

  -- do the same for root widget list
  i = 1
  for k,v in pairs(gui.widgets.childwidgets) do
    if v == wnd then
      --i = k
      break
    end
    i = i + 1
  end
  table.remove(gui.widgets.childwidgets, i)
  --gui.widgets.childwidgets[i] = nil
  gui.widgets:addwidget(wnd)
  --table.insert(gui.widgets.childwidgets, wnd) -- insert at bottom of list
end

---------------------------------------
-- standard window gui widget
function gui:create_window(id,x,y,w,h,modal,label)
  local shadow_size = 8
	local window = gui:create_basewidget(x-shadow_size,y-shadow_size,w+shadow_size*2,h+shadow_size*2)
	window.winid = id
	window.title_height = 24
	window.shadow_size = shadow_size
	window.state = "full" -- "full" or "compact"
	window.full_height = h+shadow_size
	window.compact_height = window.title_height+shadow_size*2
	
	-- sub-panels
	local window_label = gui:create_labelpanel(shadow_size,shadow_size,w-40-shadow_size*2,window.title_height, label)
	local window_panel = gui:create_basewidget(shadow_size,window.title_height+shadow_size,w,h-window.title_height)
	
	-- buttons
	local window_closebutton = gui:create_iconbutton(w-window.title_height+shadow_size,shadow_size,window.title_height,window.title_height,{21,11,8,8},
	                                                 function(self) self.parent:destroy() end)
	local window_sizebutton = gui:create_iconbutton(w-window.title_height*2+shadow_size,shadow_size,window.title_height,window.title_height,{22,21,7,3},
	                                                 function(self) self.parent:toggle_state() end)
	
	window.closebutton = window_closebutton
	window.sizebutton = window_sizebutton
	window.panel = window_panel
	window.label = window_label
	
	-- if the window is in modal mode ( = this window needs to close before we continue)
	window.modal = modal
	if (modal) then
	  local dimmer = gui:create_basewidget(0,0,app.width,app.height)
	  
	  function dimmer:draw(force)
	    if (self.redraw_needed or force) then
  	    local a = gfx.getalpha()
  	    local r,g,b = gfx.getcolor()
  	    gfx.setcolor(0,0,0)
  	    gfx.setalpha(0.4)
  	    gfx.drawtopleft(0,0,self.drawbox.w,self.drawbox.h, 5,5,1,1)
  	    gfx.setcolor(r,g,b)
  	    gfx.setalpha(a)
      end
    end
	  
	  window.dimmer = dimmer
	  --gui.widgets:addwidget(dimmer)
  end
  
  function window:toggle_state()
    if (self.state == "full") then
      -- toggle to compact
      self.state = "compact"
      self.panel.visible = false
      self:resize_abs(self.drawbox.w, self.compact_height)
      self.sizebutton.icon_coords = {22,25,7,7}
      --print("toggling to compact")
    else
      -- toggle to full
      self.state = "full"
      self.panel.visible = true
      self:resize_abs(self.drawbox.w, self.full_height)
      self.sizebutton.icon_coords = {22,21,7,3}
      --print("toggling to full")
    end
  end
	
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
	  
	  -- if modal, remove dimmer
	  if (self.modal) then
	    self.dimmer:destroy()
    end
	  
	  gui.windows:remove(self)
	  self:superdestroy()
  end
	
	--[[window_panel.super_draw = window_panel.draw
	function window_panel:draw(force)
	  if (self.redraw_needed or force) then
  		gfx.translate(self.drawbox.x,self.drawbox.y)
	
  		for k,v in pairs(self.childwidgets) do
  			v:draw(force)
  		end

  		gfx.translate(-self.drawbox.x,-self.drawbox.y)
  	end
	end]]

	function window:draw(force)
	  if (self.redraw_needed or force) then
  		gfx.translate(self.drawbox.x,self.drawbox.y)
  		
  		-- shadow
  		gfx.drawtopleft(0, 0, 18, 18, 31, 9, 18, 18) -- topleft
  		gfx.drawtopleft(self.drawbox.w - 18, 0, 18, 18, 49, 9, 18, 18) -- topright
  		gfx.drawtopleft(0, self.drawbox.h-18, 18, 18, 31, 27, 18, 18) -- bottomleft
  		gfx.drawtopleft(self.drawbox.w - 18, self.drawbox.h-18, 18, 18, 49, 27, 18, 18) -- bottomright
  		
  		gfx.drawtopleft(18, 0, self.drawbox.w-18*2, 18, 49, 9, 1, 18) -- top
  		gfx.drawtopleft(self.drawbox.w-18, 18, 18, self.drawbox.h-18*2, 49, 27, 18, 1) -- right
  		gfx.drawtopleft(18, self.drawbox.h-18, self.drawbox.w-18*2, 18, 49, 27, 1, 18) -- bottom
  		gfx.drawtopleft(0, 18, 18, self.drawbox.h-18*2, 31, 27, 18, 1) -- left
  		
  		gfx.translate(self.shadow_size, self.shadow_size)
  		
  		-- bg
  		local a = gfx.getalpha()
  		gfx.setalpha(0.9)
  		gfx.drawtopleft(1, 1, self.drawbox.w-2-self.shadow_size*2, self.drawbox.h-2-self.shadow_size*2, 2, 2, 1, 1)
  		gfx.setalpha(a)
  		
  		-- borders
  		gfx.drawtopleft(1,0,self.drawbox.w-2-self.shadow_size*2,1,1,5,1,1) -- top
  		gfx.drawtopleft(self.drawbox.w-1-self.shadow_size*2,1,1,self.drawbox.h-2-self.shadow_size*2,1,5,1,1) -- right
  		gfx.drawtopleft(1,self.drawbox.h-1-self.shadow_size*2,self.drawbox.w-2-self.shadow_size*2,1,1,5,1,1) -- bottom
  		gfx.drawtopleft(0,1,1,self.drawbox.h-2-self.shadow_size*2,1,5,1,1) -- right
  		
  		-- titlebar
  		gfx.drawtopleft(1, 1, self.drawbox.w-self.shadow_size*2-2, window.title_height-2, 508, 1, 0, 128)
	
	    gfx.translate(-self.shadow_size, -self.shadow_size)
	    
  		for k,v in pairs(self.childwidgets) do
  			v:draw(force)
  		end
  		
  		gfx.translate(-self.drawbox.x,-self.drawbox.y)
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
