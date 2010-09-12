-- creates scrollable panel (scrollable in all directions)
--[[function gui:create_panel(x,y,w,h)
  local wid = gui:create_basewidget(x,y,w,h)
  
  function wid:mousedrag(dx,dy,button)
    if (button == inp.MOUSE_MIDDLE) then
      self:needsredraw()
      self.drawbox.x = self.drawbox.x + dx
      self.drawbox.y = self.drawbox.y + dy
      self.hitbox.x = self.hitbox.x + dx
      self.hitbox.y = self.hitbox.y + dy
    end
  end
  
  wid.superdraw = wid.draw
  function wid:draw()
    if (self.redraw_needed) then
      local r,g,b = gfx.getcolor()
      gfx.setcolor(46/256,46/256,46/256)
      gfx.drawtopleft(self.drawbox.x, self.drawbox.y, self.drawbox.w, self.drawbox.h,
                      18,2,1,1)
      gfx.setcolor(r,g,b)
    end
    self:superdraw()
  end
    
  return wid
end]]

-- scrollable panel
function gui:create_horisontalpanel(x,y,w,h,max)
  local wid = gui:create_basewidget(x,y,w,h)
  wid.offset = 0
  wid.max = max
  
  function wid:mousedrag(dx,dy,button)
    if (button == inp.MOUSE_MIDDLE) then -- TODO: change this to MOUSE_MIDDLE
      --self.drawbox.x = self.drawbox.x + dx
      self.offset = self.offset + dx
      if (self.offset < -self.max) then
        self.offset = -self.max
      elseif (self.offset > 0) then
        self.offset = 0
      end
      self:needsredraw()
    end
  end

  function wid:needsredraw()
    local x,y = self:find_abspos(self)
    gui:redraw(x, y, self.drawbox.w, self.drawbox.h)
    self.redraw_needed = true
    
    -- notify parent
    if not (self.parent == nil) then
      self.parent:childisredrawn()
    end
  end
  
  function wid:find_abspos(sender)
    local x,y
    x = self.drawbox.x
    if not (sender == self) then
      x = x + self.offset
    end
    y = self.drawbox.y
    
    if not (self.parent == nil) then
      local tx,ty = self.parent:find_abspos(sender)
      x = x + tx
      y = y + ty
    end
    
    return x,y
  end
  
  --wid.superdraw = wid.draw
  function wid:draw(force)
    if (self.redraw_needed or force) then
      local r,g,b = gfx.getcolor()
      gfx.setcolor(46/256,46/256,46/256)
      gfx.drawtopleft(self.drawbox.x, self.drawbox.y, self.drawbox.w, self.drawbox.h,
                      18,2,1,1)
      gfx.setcolor(r,g,b)
    end
    gfx.translate(self.drawbox.x + self.offset, self.drawbox.y)
    for k,v in pairs(self.childwidgets) do
      v:draw(force)
    end
    gfx.translate(-(self.drawbox.x + self.offset), -self.drawbox.y)
  end
  
  function wid:find_mousehit(mx,my)
    if (self:hittest(mx,my,mx,my)) then
      local thit = nil
      --for k,v in pairs(self.childwidgets) do
      for i = #self.childwidgets, 1, -1 do
        local v = self.childwidgets[i]
        thit = v:find_mousehit(mx - (self.hitbox.x + self.offset), my - self.hitbox.y)
        
        if not (thit == nil) then
          -- we hit a child widget, return this one instead
          return thit
        end
      end
      
      return self
    end
    
    return nil
  end
    
  return wid
end

-- creates a widget stack that displays all widgets as a "stack"
function gui:create_verticalstack(x,y,w)
  local wid = gui:create_basewidget(x,y,w,10)
  
  function wid:addwidget(cwid)
    cwid.parent = self
    local offsety = 0
    for k,v in pairs(self.childwidgets) do
      offsety = offsety + v.drawbox.h
    end
    cwid:move_abs(0, offsety)
    offsety = offsety + cwid.drawbox.h
    self:resize_abs(self.drawbox.w, offsety)
    table.insert(self.childwidgets, cwid)
  end
  
  function wid:child_resized(cwid)
    local offsety = 0
    for k,v in pairs(self.childwidgets) do
      v:move_abs(v.drawbox.x, offsety)
      offsety = offsety + v.drawbox.h
    end
    self:needsredraw()
    self:resize_abs(self.drawbox.w, offsety)
    self:needsredraw()
  end
  
  function wid:childisredrawn()
    if not self.redraw_needed then
      self:needsredraw()
    end
  end
  
  --[[function wid:find_mousehit(mx,my)
    if (self:hittest(mx,my,mx,my)) then
      local thit = nil
      local total_offset = 0
      for k,v in pairs(self.childwidgets) do
        thit = v:find_mousehit(mx - self.hitbox.x, my - (self.hitbox.y + total_offset))
        total_offset = total_offset + v.drawbox.h
        
        if not (thit == nil) then
          -- we hit a child widget, return this one instead
          return thit
        end
      end
      
      return self
    end
    
    return nil
  end]]
  
  return wid
end

-- creates a widget stack that displays all widgets as a "stack" but sideways
function gui:create_horisontalstack(x,y,w,h)
  local wid = gui:create_basewidget(x,y,w,h)
  
  function wid:addwidget(cwid)
    cwid.parent = self
    local offsetx = 0
    for k,v in pairs(self.childwidgets) do
      offsetx = offsetx + v.drawbox.w
    end
    cwid:move_abs(offsetx, 0)
    offsetx = offsetx + cwid.drawbox.w
    --self:resize_abs(offsetx, self.drawbox.h)
    table.insert(self.childwidgets, cwid)
  end
  
  function wid:child_resized(cwid)
    local offsetx = 0
    for k,v in pairs(self.childwidgets) do
      v:move_abs(v.drawbox.x, offsetx)
      offsetx = offsetx + v.drawbox.w
    end
    --self:needsredraw()
    --self:resize_abs(offsetx, self.drawbox.h)
    self:needsredraw()
  end
  
  function wid:childisredrawn()
    if not self.redraw_needed then
      self:needsredraw()
    end
  end
  
  return wid
end


-- simple console output
function gui:create_console(x,y,w,h,visible)
  local wid = gui:create_basewidget(x,y,w,h)
  wid.visible = visible
  wid.stdheight = h
  wid.consolelines = {}
  
  function wid:addline(str)
    table.insert(self.consolelines, str)
  end
  
  function wid:draw(force)
    gfx.translate(self.drawbox.x, self.drawbox.y)
    if (self.redraw_needed or force) then
      --panic.text("wut", 16, 16)
      local r,g,b = gfx.getcolor()
      gfx.setcolor(26/256,26/256,26/256)
      gfx.drawtopleft(0, 0, self.drawbox.w, self.drawbox.h,18,2,1,1)
      gfx.setcolor(r,g,b)
      
      if (self.visible) then
        
        for k,v in pairs(self.consolelines) do
          panic.text(v, 16, 16*k)
        end
      else
        -- TODO: Draw something special when hidden?
      end
    end
    gfx.translate(-self.drawbox.x, -self.drawbox.y)
  end
  
  function wid:mouserelease(mx,my,button)
    if (button == inp.MOUSE_LEFT) then
      self.visible = not self.visible
      
      if (self.visible) then
        --self.drawbox.h = self.stdheight
        --self.hitbox.h = self.stdheight
        self:resize_abs(self.drawbox.w, self.stdheight)
      else
        --self.drawbox.h = 10
        --self.hitbox.h = 10
        self:resize_abs(self.drawbox.w, 10)
      end
      
      self:needsredraw()
    end
  end
  
  return wid
end

-- aoeu
function gui:create_container(x,y,w,h)
	local base_widget = gui:create_basewidget(x,y,w,h)

	return base_widget
end

function gui:create_labelpanel(x,y,w,h,text)
	local base_widget = gui:create_basewidget(x,y,w,h)

	base_widget.superdraw = base_widget.draw

	function base_widget:draw()
		gfx.translate(self.drawbox.x,self.drawbox.y)

		gui:drawfont(text, x, y)

		gfx.translate(-self.drawbox.x,-self.drawbox.y)

		base_widget:superdraw()
	end

	return base_widget
end

function gui:create_movablewindow(x,y,w,h)
	local base_window = gui:create_movablepanel(x,y,w,h)
	local minimize_button = gui:create_staticpanel(w-40,0,20,20)
	local close_button = gui:create_staticpanel(w-20,0,20,20)
	local window_label = gui:create_labelpanel(6,6,0,0,"SUKEEEH")
	local minimize_label_arrow = gui:create_labelpanel(0,0,0,0,">")
	local close_label_icon = gui:create_labelpanel(6,5,0,0,"x")

	function base_window:destroy_self()
		self = nil
	end

	minimize_label_arrow.super_draw = minimize_label_arrow.draw
	close_label_icon.super_draw = close_label_icon.draw

	local top_container = gui:create_container(0,0,w,20)

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
		local move_offset = {x = 11, y = -11}
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
	end

	function base_window:draw()
		self:superdraw()

		gfx.translate(self.hitbox.x,self.hitbox.y)
	
		for k,v in pairs(self.childwidgets) do
			v:draw()
		end

		gfx.translate(-self.hitbox.x,-self.hitbox.y)
	end

	return base_window
end

function gui:create_staticpanel(x,y,w,h)
	local base_widget = gui:create_basewidget(x,y,w,h)

	base_widget.superdraw = base_widget.draw

	function base_widget:draw()
		gfx.translate(self.drawbox.x,self.drawbox.y)

		-- bg
		gfx.drawtopleft(2, 2, self.drawbox.w-4, self.drawbox.h-4,
						512,1,1,254)
                    
		-- topleft
		gfx.drawtopleft(0, 0, 5, 5,
						0,0,5,5)
    
		-- topright
		gfx.drawtopleft(self.drawbox.w-5, 0, 5, 5,
						9,0,5,5)
                    
		-- top
		gfx.drawtopleft(5, 0, self.drawbox.w-10, 5,
						5,0,1,5)
                    

		-- bottomleft
		gfx.drawtopleft(0, self.drawbox.h-5, 5, 5,
						0,9,5,5)

		-- bottomright
		gfx.drawtopleft(self.drawbox.w-5, self.drawbox.h-5, 5, 5,
						9,9,5,5)
                    
		-- top
		gfx.drawtopleft(5, self.drawbox.h-5, self.drawbox.w-10, 5,
						5,9,1,5)
                    
		-- left
		gfx.drawtopleft(0, 5, 5, self.drawbox.h-10,
						0,5,5,1)
                    
		-- right
		gfx.drawtopleft(self.drawbox.w-5, 5, 5, self.drawbox.h-10,
						9,5,5,1)

		gfx.translate(-self.drawbox.x,-self.drawbox.y)

		self:superdraw()
	end

	return base_widget
end

function gui:create_movablepanel(x,y,w,h)
	local base_widget = gui:create_basewidget(x,y,w,h)
	--base_widget.offset = { dx = 0, dy = 0 }

	function base_widget:mousedrag(dx,dy,button)
		if (button == inp.MOUSE_LEFT) then
		  self:needsredraw()
			--self.offset.dx = self.offset.dx + dx
			--self.offset.dy = self.offset.dy + dy

			self.drawbox.x = self.drawbox.x + dx
			self.drawbox.y = self.drawbox.y + dy
			self.hitbox.x = self.hitbox.x + dx
			self.hitbox.y = self.hitbox.y + dy
			self:needsredraw()
		end
	end

	function base_widget:draw()
		gfx.translate(self.drawbox.x,self.drawbox.y)

		-- bg
		gfx.drawtopleft(2, 2, self.drawbox.w-4, self.drawbox.h-4,
						512,1,1,254)
                    
		-- topleft
		gfx.drawtopleft(0, 0, 5, 5,
						0,0,5,5)
    
		-- topright
		gfx.drawtopleft(self.drawbox.w-5, 0, 5, 5,
						9,0,5,5)
                    
		-- top
		gfx.drawtopleft(5, 0, self.drawbox.w-10, 5,
						5,0,1,5)
                    

		-- bottomleft
		gfx.drawtopleft(0, self.drawbox.h-5, 5, 5,
						0,9,5,5)

		-- bottomright
		gfx.drawtopleft(self.drawbox.w-5, self.drawbox.h-5, 5, 5,
						9,9,5,5)
                    
		-- top
		gfx.drawtopleft(5, self.drawbox.h-5, self.drawbox.w-10, 5,
						5,9,1,5)
                    
		-- left
		gfx.drawtopleft(0, 5, 5, self.drawbox.h-10,
						0,5,5,1)
                    
		-- right
		gfx.drawtopleft(self.drawbox.w-5, 5, 5, self.drawbox.h-10,
						9,5,5,1)

		gfx.translate(-(self.drawbox.x),-(self.drawbox.y))
	end
	
	return base_widget
end

-- simple button aoeu
function gui:create_simplebutton(x,y,w,h,label,action)
  local wid = gui:create_basewidget(x,y,w,h)
  wid.action = action
  wid.label = label
  wid.state = 0 -- 0 = up, 1 = down
  
  function wid:mouserelease(mx,my,button)
    if (button == inp.MOUSE_LEFT) then
      self:action(mx,my,button)
    end
    self.state = 0
    self:needsredraw()
  end
  
  function wid:mousepush(mx,my,button)
    if (button == inp.MOUSE_LEFT) then
      self.state = 1
      self:needsredraw()
    end
  end
  
  function wid:draw(force)
    if (self.redraw_needed or force) then
      gfx.translate(self.drawbox.x, self.drawbox.y)
    
      -- bg
      if (self.state == 0) then
        gfx.drawtopleft(2, 2, self.drawbox.w-4, self.drawbox.h-4,
                        512,1,1,254)
      else
        gfx.drawtopleft(2, 2, self.drawbox.w-4, self.drawbox.h-4,
                        510,1,0,254)
      end
                    
      -- topleft
      gfx.drawtopleft(0, 0, 4, 4,
                      0,0,4,4)
    
      -- topright
      gfx.drawtopleft(self.drawbox.w-4, 0, 4, 4,
                      10,0,4,4)
                    
      -- top
      gfx.drawtopleft(4, 0, self.drawbox.w-8, 4,
                      7,0,1,4)
                    

      -- bottomleft
      gfx.drawtopleft(0, self.drawbox.h-4, 4, 4,
                      0,10,4,4)

      -- bottomright
      gfx.drawtopleft(self.drawbox.w-4, self.drawbox.h-4, 4, 4,
                      10,10,4,4)
                    
      -- bottom
      gfx.drawtopleft(4, self.drawbox.h-4, self.drawbox.w-8, 4,
                      7,10,1,4)
                    
      -- left
      gfx.drawtopleft(0, 4, 4, self.drawbox.h-8,
                      0,7,4,1)
                    
      -- right
      gfx.drawtopleft(self.drawbox.w-4, 4, 4, self.drawbox.h-8,
                      10,7,4,1)
      
      -- label
      if (self.state == 0) then
        gui:drawcenteredfont(self.label, self.drawbox.w / 2 - 1, self.drawbox.h / 2)
      else
        gui:drawcenteredfont(self.label, self.drawbox.w / 2, self.drawbox.h / 2 + 1)
      end
    
      gfx.translate(-self.drawbox.x, -self.drawbox.y)
    
    end
  end
  
  return wid
end

function gui:create_menubar(x,y,w)
  local wid = gui:create_horisontalstack(x,y,w,25)
  
  wid.superdraw = wid.draw
  function wid:draw(force)
    if (self.redraw_needed or force) then
      gfx.translate(self.drawbox.x, self.drawbox.y)
    
      -- bg
      local r,g,b = gfx.getcolor()
      gfx.setcolor(0.2,0.2,0.2)
      gfx.drawtopleft(0, 0, self.drawbox.w, self.drawbox.h,
                      20,6,1,1)
      gfx.setcolor(r,g,b)
    
      gfx.translate(-self.drawbox.x, -self.drawbox.y)
    
      self:superdraw(force)
    end
  end
  
  return wid
end

function gui:create_menubutton(label,menu)
  local wid = gui:create_basewidget(0,0,8*#label+20,25)
  wid.label = label
  wid.menu = menu
  
  function wid:mouserelease(mx,my,button)
    if (button == inp.MOUSE_LEFT) then
      gui:spawn_menu(mx,my,self.menu)
    end
    self:needsredraw()
  end
  
  function wid:mousepush(mx,my,button)
    if (button == inp.MOUSE_LEFT) then
      --self:needsredraw()
    end
  end
  
  function wid:draw(force)
    if (self.redraw_needed or force) then
      gfx.translate(self.drawbox.x, self.drawbox.y)
    
      -- bg
      --[[if (self.state == 0) then
        gfx.drawtopleft(0, 0, self.drawbox.w, self.drawbox.h,
                        512,1,1,254)
      else
        gfx.drawtopleft(0, 0, self.drawbox.w, self.drawbox.h,
                        510,1,0,254)
      end]]
      
      -- label
      if (self.state == 0) then
        gui:drawcenteredfont(self.label, self.drawbox.w / 2 - 1, self.drawbox.h / 2)
      else
        gui:drawcenteredfont(self.label, self.drawbox.w / 2, self.drawbox.h / 2 + 1)
      end
    
      gfx.translate(-self.drawbox.x, -self.drawbox.y)
    
    end
  end
  
  return wid
end

-- spawns a menu in the root of the widget tree
function gui:spawn_menu(x,y,menu)
  local wid = gui:create_menu(x,y,menu)
  gui.widgets:addwidget(wid)
end

function gui:spawn_submenu(root,parent,x,y,menu)
  local wid = gui:create_menu(x,y,menu)
  wid.menu_parent = parent
  wid.menu_root = root
  parent.menu_child = wid
  gui.widgets:addwidget(wid)
end

-- creates a menu
function gui:create_menu(x,y,menu)
  local wid = gui:create_basewidget(x,y,200,10)
  wid.widget_type = "menu"
  wid.menu_parent = nil
  wid.menu_child = nil
  wid.menu_root = nil
  wid.stdwith = 200
  wid.itemheight = 24
  wid.menu = menu
  wid.highlightid = 0
  
  -- overflow control
  if (x + wid.stdwith > app.width) then
    wid.drawbox.x = app.width - wid.stdwith
    wid.hitbox.x = wid.drawbox.x
  end
  
  -- set correct height of menu
  local new_h = 0
  for k,v in pairs(menu) do
    new_h = new_h + wid.itemheight
  end
  wid.drawbox.h = new_h
  wid.hitbox.h = new_h
  
  wid.superdestroy = wid.destroy
  function wid:destroy()
    -- call sub menus to also destroy
    if (self.menu_child) then
      self.menu_child:destroy()
    end
    
    self:superdestroy()
  end
  
  function wid:mouseover(mx,my)
    
    -- if we have child menus, close them
    --[[if (self.menu_child) then
      self.menu_child:destroy()
    end]]
    
    -- find correct menu item
    local dh = my - self.drawbox.y
    local i = math.ceil((dh / self.drawbox.h) * #self.menu)
    if not (i == self.highlightid) then
      self.highlightid = i
      self:needsredraw()
      if (self.menu[i]) then
        if (self.menu_child) then
          self.menu_child:destroy()
        end
        
        if (type(self.menu[i][3]) == "table") then
          
          local t_menu_root = self.menu_root
          if (t_menu_root == nil) then
            t_menu_root = self
          end
          gui:spawn_submenu(t_menu_root, -- "menu root"
                            self, -- "menu parent"
                            self.drawbox.x+self.drawbox.w, -- x position
                            self.drawbox.y+(i-1)*self.itemheight+self.itemheight/2, -- y position
                            self.menu[i][3]) -- menu table
          
        end
      end
    end
  end
  
  function wid:lostfocus(newfocus)
    if (newfocus.widget_type == "menu") then
      return 0
    end
    
    if (self.menu_parent) then
      self.menu_parent:lostfocus(newfocus)
    end
    
    self:destroy()
  end
  
  function wid:mouserelease(mx,my,button)
    if (button == inp.MOUSE_LEFT) then
      -- if we have child menus, close them
      if (self.menu_child) then
        self.menu_child:destroy()
      end
      
      -- find correct menu item
      local dh = my - self.drawbox.y
      local i = math.ceil((dh / self.drawbox.h) * #self.menu)
      if (self.menu[i]) then
        if (type(self.menu[i][3]) == "function") then
          self.menu[i][3](self,mx, my, button)
          
          -- close the whole tree!
          if (self.menu_root) then
            self.menu_root:destroy()
          else
            self:destroy() -- we are the root
          end
        
          self:needsredraw()
        elseif (type(self.menu[i][3]) == "table") then

          local t_menu_root = self.menu_root
          if (t_menu_root == nil) then
            t_menu_root = self
          end
          gui:spawn_submenu(t_menu_root, -- "menu root"
                            self, -- "menu parent"
                            self.drawbox.x+self.drawbox.w, -- x position
                            self.drawbox.y+(i-1)*self.itemheight+self.itemheight/2, -- y position
                            self.menu[i][3]) -- menu table
          
        end
      end
    end
    
  end
  
  function wid:mousepush(mx,my,button)
    if (button == inp.MOUSE_LEFT) then
      self:needsredraw()
    end
  end
  
  wid.superdraw = wid.draw
  function wid:draw(force)
    if (self.redraw_needed or force) then
      gfx.translate(self.drawbox.x, self.drawbox.y)
    
      -- bg
      gfx.drawtopleft(2, 0, self.drawbox.w-4, self.drawbox.h,
                      40,6,1,1)
                    
      gfx.drawtopleft(0, 2, self.drawbox.w, self.drawbox.h-4,
                      40,6,1,1)
                      
      -- topleft
      gfx.drawtopleft(0, 0, 2, 2,
                      35,0,2,2)
      
      -- topright
      gfx.drawtopleft(self.drawbox.w-2, 0, 2, 2,
                      45,0,2,2)
      
      -- bottomleft
      gfx.drawtopleft(0, self.drawbox.h-2, 2, 2,
                      35,10,2,2)

      -- bottomright
      gfx.drawtopleft(self.drawbox.w-2, self.drawbox.h-2, 2, 2,
                      45,10,2,2)
                      
      -- loop through all menu items
      local item_y = 0
      for k,v in pairs(self.menu) do
        if (k == self.highlightid) then
          local r,g,b = gfx.getcolor()
          gfx.setcolor(0.2,0.2,0.2)
          gfx.drawtopleft(0, item_y+2, self.drawbox.w, self.itemheight-4,
                          20,1,1,1)
          gfx.setcolor(r,g,b)
        end
        
        -- item
        gui:drawfont(v[1], 10, item_y + 12)
        
        -- short
        if (v[2]) then
          local r,g,b = gfx.getcolor()
          gfx.setcolor(0.5, 0.5, 0.5)
          gui:drawfont(v[2], self.stdwith-(#v[2]*8), item_y + 12)
          gfx.setcolor(r,g,b)
        elseif (type(v[3]) == "table") then
          gui:drawfont(">", self.stdwith-8, item_y + 12)
        end
        
        item_y = item_y + self.itemheight
      end
    
      gfx.translate(-self.drawbox.x, -self.drawbox.y)
      
      --self:superdraw(force)
      
    end
  end
  
  return wid
  --gui.widgets:addwidget(wid)
end

