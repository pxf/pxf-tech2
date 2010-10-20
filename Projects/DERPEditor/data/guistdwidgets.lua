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
function gui:create_horizontalpanel(x,y,w,h,max)
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
  
  -- wid.superdraw = wid.draw
  function wid:draw(force)
    if self.visible then
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
  end
  
  function wid:find_mousehit(mx,my)
    if (self:hittest(mx,my,mx,my)) then
      local thit = nil
      for k,v in pairs(self.childwidgets) do
      --for i = #self.childwidgets, 1, -1 do
        --local v = self.childwidgets[i]
        local htest = v:find_mousehit(mx - (self.hitbox.x + self.offset), my - self.hitbox.y)
        if htest then
          thit = htest
        end
        
      end
      
      if not (thit == nil) then
        -- we hit a child widget, return this one instead
        return thit
      end
      
      return self
    end
    
    return nil
  end
    
  return wid
end

-- creates a widget stack that displays all widgets as a "stack"
function gui:create_verticalstack(x,y,w,h)
  local wid = gui:create_basewidget(x,y,w,h)
  
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
function gui:create_horizontalstack(x,y,w,h)
  local wid = gui:create_basewidget(x,y,w,h)
  
  function wid:addwidget(cwid,location)
  
    cwid.parent = self
    local offsetx = 0
    for k,v in pairs(self.childwidgets) do
      offsetx = offsetx + v.drawbox.w
    end
    cwid:move_abs(offsetx, 0)
    offsetx = offsetx + cwid.drawbox.w
    --self:resize_abs(offsetx, self.drawbox.h)
    table.insert(self.childwidgets, cwid)
	
	--print(cwid.widget_type .. ": " .. offsetx)
  end
  
   function wid:removewidget(cwid)
	local find_k = nil
	
	if not cwid then
		return nil
	end
	
	-- make sure we find
	for k,v in pairs(self.childwidgets) do
		if (v == cwid) then
			find_k = k
			break
		end
	end
	
	if find_k then
		--self.childwidgets[find_k] = nil
		table.remove(self.childwidgets, find_k)
		
		local offset_x = 0
		for k,v in pairs(self.childwidgets) do
			
		end
	end
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

function gui:create_statusbar(x,y,w,h,default_text)
  local wid = gui:create_basewidget(x,y,w,h)
  wid.default_text = default_text
  wid.text = default_text
  
  function wid:settext(text)
    if (text == nil) then
      self.text = self.default_text
    else
      self.text = text
    end
  end
  
  function wid:draw(force)
    if (self.redraw_needed or force) then
      gfx.translate(self.drawbox.x, self.drawbox.y)
      
      local r,g,b = gfx.getcolor()
      gfx.setcolor(0.6,0.6,0.6)
      gui:drawfont(self.text,24,12)
      gfx.setcolor(r,g,b)

      gfx.translate(-self.drawbox.x, -self.drawbox.y)
    end
  end
  
  return wid
end


-- simple console output
function gui:create_console(x,y,w,h,open_state)
  local wid = gui:create_basewidget(x,y,w,h)
  wid.open_state = open_state
  wid.stdheight = h
  wid.minheight = 5
  wid.consolelines = {}
  
  if open_state then
    wid.drawbox.h = wid.stdheight
    wid.hitbox.h = wid.stdheight
  else
    wid.drawbox.h = wid.minheight
    wid.hitbox.h = wid.minheight
  end
  
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
      
      if (self.open_state) then
        
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
      self.open_state = not self.open_state
      
      if (self.open_state) then
        --self.drawbox.h = self.stdheight
        --self.hitbox.h = self.stdheight
        self:resize_abs(self.drawbox.w, self.stdheight)
      else
        --self.drawbox.h = 10
        --self.hitbox.h = 10
        self:resize_abs(self.drawbox.w, self.minheight)
      end
      
      self:needsredraw()
    end
  end
  
  return wid
end

function gui:create_centeredmultiline_label(x,y,w,h,lines)
	local base_widget = gui:create_basewidget(x,y,w,h)
	base_widget.lines = lines

	base_widget.superdraw = base_widget.draw
	function base_widget:draw()
		gfx.translate(self.drawbox.x,self.drawbox.y)

    for k,v in pairs(self.lines) do
      gui:drawcenteredfont(v, self.drawbox.w / 2, 12+16*(k-1))
    end

		gfx.translate(-self.drawbox.x,-self.drawbox.y)

		base_widget:superdraw()
	end

	return base_widget
end

function gui:create_multiline_label(x,y,w,h,lines)
	local base_widget = gui:create_basewidget(x,y,w,h)
	base_widget.lines = lines

	base_widget.superdraw = base_widget.draw
	function base_widget:draw()
		gfx.translate(self.drawbox.x,self.drawbox.y)

    local max_len = math.floor(w / 8)
    for k,v in pairs(self.lines) do
      local len = gui:get_font_length(v)
      local out_str = v
      if (len > max_len) then
        out_str = string.sub(out_str, 1, max_len)
      end
      gui:drawfont(tostring(out_str), 12, 12+16*(k-1))
    end

		gfx.translate(-self.drawbox.x,-self.drawbox.y)

		base_widget:superdraw()
	end

	return base_widget
end

function gui:create_centeredlabelpanel(x,y,w,h,text)
	local base_widget = gui:create_basewidget(x,y,w,h)
	base_widget.label_text = text

	base_widget.superdraw = base_widget.draw
	function base_widget:draw()
		gfx.translate(self.drawbox.x,self.drawbox.y)

    local maxlen = math.ceil(self.drawbox.w / 12) - 3
    local shortlabel = string.sub(self.label_text,-maxlen)
    if (#shortlabel < #self.label_text) then
      shortlabel = "..." .. shortlabel
    end

		gui:drawcenteredfont(shortlabel, self.drawbox.w / 2, 12)

		gfx.translate(-self.drawbox.x,-self.drawbox.y)

		base_widget:superdraw()
	end

	return base_widget
end


function gui:create_labelpanel(x,y,w,h,text)
	local base_widget = gui:create_basewidget(x,y,w,h)
	base_widget.label_text = text

	base_widget.superdraw = base_widget.draw

	function base_widget:draw()
		gfx.translate(self.drawbox.x,self.drawbox.y)

		gui:drawfont(self.label_text, 12, self.drawbox.h / 2)

		gfx.translate(-self.drawbox.x,-self.drawbox.y)

		base_widget:superdraw()
	end
	
	function base_widget:find_mousehit(mx,my)
    if (self:hittest(mx,my,mx,my)) then
      local thit = nil
      for k,v in pairs(self.childwidgets) do
      --for i = #self.childwidgets, 1, -1 do
        --local v = self.childwidgets[i]
        if not (v == nil) then
          local htest = v:find_mousehit(mx - self.hitbox.x, my - self.hitbox.y)
          if htest then
            thit = htest
          end
        end
        
      end
      
      if not (thit == nil) then
        -- we hit a child widget, return this one instead
        return thit
      end
      
      return nil
    end
    
    return nil
  end

	return base_widget
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

	function base_widget:mousedrag(dx,dy,button)
		if (button == inp.MOUSE_LEFT) then
			if (gui.snap_to_grid) then
				local x,y = inp.getmousepos()
				
				local cell_x = math.ceil(x / gui.grid_size)
				local cell_y = math.ceil(y / gui.grid_size)
				
				-- Note: add offset from point clicked within the component
				-- Right now, its just moving the box
				
				self:needsredraw()
				self.drawbox.x = cell_x * gui.grid_size
				self.hitbox.x = cell_x * gui.grid_size
				self.drawbox.y = cell_y * gui.grid_size
				self.hitbox.y = cell_y * gui.grid_size
				self:needsredraw()
				
				
			else
				self:needsredraw()
				self.drawbox.x = self.drawbox.x + dx
				self.drawbox.y = self.drawbox.y + dy
				self.hitbox.x = self.hitbox.x + dx
				self.hitbox.y = self.hitbox.y + dy
				self:needsredraw()
			end
			
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

-- simple button with label
function gui:create_labelbutton(x,y,w,h,label,action)
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
			gfx.drawtopleft(2,2,self.drawbox.w-4,self.drawbox.h-4, 507,0,0,128) -- upper left corner
      --[[if (self.state == 0) then
        gfx.drawtopleft(2, 2, self.drawbox.w-4, self.drawbox.h-4,
                        512,1,1,254)
      else
        gfx.drawtopleft(2, 2, self.drawbox.w-4, self.drawbox.h-4,
                        510,1,0,254)
      end]]
      
      -- draw borders
			gfx.drawtopleft(0, 0, 2, 2, 5,10,2,2) -- upper left corner
			gfx.drawtopleft(self.drawbox.w - 2, 0, 2, 2,10,10,2,2) -- upper right corner
			gfx.drawtopleft(0, self.drawbox.h-2, 2, 2,5,15,2,2) -- lower left corner
			gfx.drawtopleft(self.drawbox.w-2,self.drawbox.h-2,2,2,10,15,2,2) -- lower right corner
			
			gfx.drawtopleft(0,2,2,self.drawbox.h-4,5,12,2,1) -- left frame	
			gfx.drawtopleft(self.drawbox.w-2,2,2,self.drawbox.h-4,10,12,2,1) -- right frame	
			gfx.drawtopleft(2,0,self.drawbox.w-4,2,7,10,1,2) -- upper frame
			gfx.drawtopleft(2,self.drawbox.h - 2,self.drawbox.w-4,2,7,15,1,2) -- lower frame)
                    
      -- topleft
      --[[gfx.drawtopleft(0, 0, 4, 4,
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
                      10,7,4,1)]]
      
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

-- simple button with icon
function gui:create_iconbutton(x,y,w,h,icon_coords,action)
  local wid = gui:create_basewidget(x,y,w,h)
  wid.action = action
  wid.icon_coords = icon_coords
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
      --[[if (self.state == 0) then
        gfx.drawtopleft(2, 2, self.drawbox.w-4, self.drawbox.h-4,
                        512,1,1,254)
      else
        gfx.drawtopleft(2, 2, self.drawbox.w-4, self.drawbox.h-4,
                        510,1,0,254)
      end]]
      --gfx.drawtopleft(0, 0, self.drawbox.w, self.drawbox.h, 2, 2, 1, 1)
      
      -- icon
      if (self.state == 0) then
        gfx.drawcentered(self.drawbox.w / 2, self.drawbox.h / 2, self.icon_coords[3], self.icon_coords[4]
                        , self.icon_coords[1], self.icon_coords[2], self.icon_coords[3], self.icon_coords[4]) -- texcoords
      else
        gfx.drawcentered(self.drawbox.w / 2 + 1, self.drawbox.h / 2 + 1, self.icon_coords[3], self.icon_coords[4]
                        , self.icon_coords[1], self.icon_coords[2], self.icon_coords[3], self.icon_coords[4]) -- texcoords
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

function gui:spawn_primenu(parent,x,y,menu)
  local wid = gui:create_menu(x,y,menu)
  wid.menu_parent = parent
  wid.menu_root = parent
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
	
	if self.destroy_callback then
		self:destroy_callback()
	end
  end
  
  function wid:mouseover(mx,my)
    
    
    -- find correct menu item
    local dh = my - self.drawbox.y
    local i = math.ceil((dh / self.drawbox.h) * #self.menu)

    if not (i == self.highlightid) then
      self.highlightid = i
      self:needsredraw()
      if (self.menu[i]) then
        
        -- update statusbar
        if (self.menu[i][2].tooltip) then
          gui:tooltip(self.menu[i][2].tooltip)
        end
        
        -- close all submenus
        if (self.menu_child) then
          self.menu_child:destroy()
        end
        
        -- if this is a submenu, open it!
        if not (self.menu[i][2].menu == nil) then
          
          local t_menu_root = self.menu_root
          if (t_menu_root == nil) then
            t_menu_root = self
          end
          gui:spawn_submenu(t_menu_root, -- "menu root"
                            self, -- "menu parent"
                            self.drawbox.x+self.drawbox.w, -- x position
                            self.drawbox.y+(i-1)*self.itemheight, -- y position
                            self.menu[i][2].menu) -- menu table
          
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
        if (self.menu[i][2].menu == nil) then
          -- clickable menu item
          self.menu[i][2]:onclick(self,mx, my, button)
          
    	    if ( not (self.menu[i][2].toggle == nil) ) then
      			-- execute function
      			self.menu[i][2].toggle = not self.menu[i][2].toggle

          -- close the whole tree!
          elseif (self.menu_root) then
            self.menu_root:destroy()
          else
            self:destroy() -- we are the root
          end
        
          self:needsredraw()
        else
          -- submenu
          local t_menu_root = self.menu_root
          if (t_menu_root == nil) then
            t_menu_root = self
          end
          gui:spawn_submenu(t_menu_root, -- "menu root"
                            self, -- "menu parent"
                            self.drawbox.x+self.drawbox.w, -- x position
							self.drawbox.y, -- y position
                            --self.drawbox.y+(i-1)*self.itemheight+self.itemheight/2, -- y position
                            self.menu[i][2].menu) -- menu table
          
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
		local old_a = gfx.getalpha()
		
		gfx.translate(self.drawbox.x, self.drawbox.y)
		
		gfx.setalpha(0.8)
		gfx.drawtopleft(1,1,self.drawbox.w-2,self.drawbox.h-2,510, 0, 1, 128)
		
		
		-- borders
		local r,g,b = gfx.getcolor()
		gfx.setcolor(0.278431373,0.215686275,0.0941176471)
		
		gfx.drawtopleft(1,0,self.drawbox.w-2,1,5,5,1,1) -- top
		gfx.drawtopleft(self.drawbox.w-1,1,1,self.drawbox.h-2,5,5,1,1) -- right
		gfx.drawtopleft(1,self.drawbox.h-1,self.drawbox.w-2,1,5,5,1,1) -- bottom
		gfx.drawtopleft(0,1,1,self.drawbox.h-2,5,5,1,1) -- right
		
		gfx.setalpha(old_a)
		gfx.setcolor(r,g,b)
		
      
                      
      -- loop through all menu items
      local item_y = 0
      for k,v in pairs(self.menu) do
        if (k == self.highlightid) then
          r,g,b = gfx.getcolor()
		  old_a = gfx.getalpha()
		  gfx.setalpha(0.6)
          gfx.setcolor(1.0,1.0,1.0)
          gfx.drawtopleft(1, item_y+1, self.drawbox.w-1, self.itemheight-1,
                          21,5,1,1)
          gfx.setcolor(r,g,b)
		  gfx.setalpha(old_a)
        end
        
        -- item
        gui:drawfont(v[1], 20, item_y + 12)
        
        -- short
        if not (v[2].shortcut == nil) then
          local r,g,b = gfx.getcolor()
          gfx.setcolor(0.5, 0.5, 0.5)
          gui:drawfont(v[2], self.stdwith-(#v[2]*8), item_y + 12)
          gfx.setcolor(r,g,b)
        elseif not (v[2].menu == nil) then
          gui:drawfont(">", self.stdwith-8, item_y + 12)
		elseif v[2].toggle then
		  gui:drawfont("*",10,item_y+14)
        end
        
        item_y = item_y + self.itemheight
      end
    
      gfx.translate(-self.drawbox.x, -self.drawbox.y)
    end
  end
  
  return wid
end

-- creates a menu
function gui:create_menubar(x,y,w,menus)
  local wid = gui:create_basewidget(x,y,w,30)
  --wid.widget_type = "menu"
  wid.stdheight = 30
  wid.stdpadding = 20
  wid.menuwidth = 0
  wid.menus = menus
  wid.menu_child = nil
  wid.highlightid = 0
  wid.state = "inactive" -- or "active"
  
  for k,v in pairs(menus) do
    wid.menuwidth = wid.menuwidth + (#v[1]*8) + wid.stdpadding
  end
  
  
  function wid:mouseover(mx,my)
    if (self.state == "active") then
      -- find correct menu item
      local dw = mx - self.drawbox.x
      local i = math.ceil((dw / self.menuwidth) * #self.menus)

      if not (i == self.highlightid) then
        self.highlightid = i
        self:needsredraw()
        if (self.menus[i]) then
        
          -- close all submenus
          if (self.menu_child) then
            self.menu_child:destroy()
          end
        
          -- get correct spawn position
          local offset = 20
          for k,v in pairs(self.menus) do          
            if (self.menus[i] == v) then
              break
            end
            offset = offset + (#v[1]*8) + self.stdpadding
          end
        
          -- this is always a submenu          
          gui:spawn_primenu(self,offset,self.drawbox.y+self.drawbox.h,self.menus[i][2])
        end
      end
    end
  end
  
  function wid:destroy()
    self.state = "inactive"
    self.highlightid = 0
    -- only destroy our child menu
    if (self.menu_child) then
      self.menu_child:destroy()
    end
    
    self:needsredraw()
  end
  
  function wid:lostfocus(newfocus)
    if (newfocus.widget_type == "menu") then
      return 0
    else
      -- menus lost focus
      self:destroy()
    end
  end
  
  function wid:mouserelease(mx,my,button)
    if (button == inp.MOUSE_LEFT) then
      
      -- find correct menu item
      local dw = mx - self.drawbox.x
      local i = math.ceil((dw / self.menuwidth) * #self.menus)
      if not (i == self.highlightid) then
        self.state = "active"
        self.highlightid = i
        self:needsredraw()
        if (self.menus[i]) then

          -- close all submenus
          if (self.menu_child) then
            self.menu_child:destroy()
          end

          -- get correct spawn position
          local offset = 20
          for k,v in pairs(self.menus) do          
            if (self.menus[i] == v) then
              break
            end
            offset = offset + (#v[1]*8) + self.stdpadding
          end

          -- this is always a submenu          
          gui:spawn_primenu(self,offset,self.drawbox.y+self.drawbox.h,self.menus[i][2])
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
      local old_a = gfx.getalpha()            
                    
      -- loop through all menu items
      local item_x = 20
      for k,v in pairs(self.menus) do
        if (k == self.highlightid) then
          
		  gfx.setalpha(0.8)
		  
		  local w = (#v[1]*8) + self.stdpadding
		  
			gfx.drawtopleft(item_x,self.drawbox.y,w,self.drawbox.h,510, 0, 1, 128)
			-- borders
			local r,g,b = gfx.getcolor()
			gfx.setcolor(0.278431373,0.215686275,0.0941176471)
			
			gfx.drawtopleft(item_x+1,self.drawbox.y,w-1,1,5,5,1,1) -- top
			gfx.drawtopleft(item_x,self.drawbox.y+1,1,self.drawbox.h-1,5,5,1,1)
			gfx.drawtopleft(item_x+w,self.drawbox.y+1,1,self.drawbox.h-1,5,5,1,1)
			gfx.drawtopleft(item_x+1,self.drawbox.y + self.drawbox.h,w-1,1,5,5,1,1)
			
			gfx.setalpha(old_a)
			gfx.setcolor(r,g,b)

          gfx.setalpha(old_a)
		  
		  gui:drawcenteredfont(v[1], item_x + ((#v[1]*8) + self.stdpadding) / 2, 15)
        else
			-- item
			local r,g,b = gfx.getcolor()
			gfx.setcolor(0.278431373,0.215686275,0.0941176471)
			gui:drawcenteredfont(v[1], item_x + ((#v[1]*8) + self.stdpadding) / 2, 15)
			gfx.setcolor(r,g,b)
		end
        
        item_x = item_x + self.stdpadding + (#v[1]*8)
      end
    
      gfx.translate(-self.drawbox.x, -self.drawbox.y)
      
    end
  end
  
  return wid
  --gui.widgets:addwidget(wid)
end

-- creates a menu
function gui:create_textinput(x,y,w,masked,stdvalue,changed) -- changed = function to be called once changed
  local wid = gui:create_basewidget(x,y,w,20)
  wid.changed = changed
  wid.masked = masked -- passwords etc
  wid.stdheight = 20
  wid.selectionheight = 16
  wid.stdpadding = 10
  wid.value = ""
  wid.old_value = nil
  if (stdvalue) then
    wid.value = stdvalue
  end
  wid.state = "normal"
  wid.selection = {start = 0, finish = nil, direction = 0}
  wid.viewstart = 0
  
  -- key states
  wid.backspace = false
  wid.deletekey = false
  wid.leftkey = false
  wid.rightekey = false
  
  wid.mouseselect = false
  
  -- find out max visible char number
  wid.maxvisible = math.floor((w-wid.stdpadding-8) / 8)
  
  function wid:sanitycheck_selection()
    if (self.selection.finish == self.selection.start) then
      self.selection.finish = nil
    end
    
    if self.selection.finish then
      
      if (self.selection.finish < self.selection.start) then
        local tstart = self.selection.start
        self.selection.start = self.selection.finish
        self.selection.finish = tstart
      end
      
      if (self.selection.finish < 0) then
        self.selection.finish = 0
      elseif (self.selection.finish > #self.value) then
        self.selection.finish = #self.value
      end
    end
    
    if (self.selection.start < 0) then
      self.selection.start = 0
    elseif (self.selection.start > #self.value) then
      self.selection.start = #self.value
    end
      
    if (self.selection.start - self.viewstart > self.maxvisible) then
      self.viewstart = self.selection.start - self.maxvisible
    end
    
    if (self.selection.start < self.viewstart) then
      self.viewstart = self.selection.start
    end
    
  end
  
  function wid:mousedrag(dx,dy,button)
    if (button == inp.MOUSE_LEFT) then
      if (self.mouseselect) then
        self:needsredraw()
        -- find selection finish
        local mx,my = inp.getmousepos()
        local dw = mx - self.drawbox.x - self.stdpadding / 2 + 4
        local i = math.floor((dw / (self.drawbox.w - self.stdpadding)) * self.maxvisible)
        local deltaselection = self.viewstart + i
        
        if (deltaselection <= self.selection.start) then
          if not (self.selection.finish) then
            self.selection.finish = self.selection.start
          end
          self.selection.start = deltaselection
        else
          self.selection.finish = deltaselection
        end
      end
      
    end
  end
  
  function wid:mouserelease(mx,my,button)
    self.mouseselect = false
  end
  
  function wid:mousepush(mx,my,button)
    if (button == inp.MOUSE_LEFT) then
      
      if not (self.mouseselect) then
        self:needsredraw()
        -- new mouse selection
        
        -- find selection start
        local dw = mx - self.drawbox.x - self.stdpadding / 2 + 4
        local i = math.floor((dw / (self.drawbox.w - self.stdpadding)) * self.maxvisible)
        self.selection.start = self.viewstart + i
        self.selection.finish = nil
        self:sanitycheck_selection()
        self.mouseselect = true
      else
        -- continued selection
      end
      
    end
  end
  
  function wid:lostfocus(wid)
    self.state = "normal"
    
    --if (self.changed) then
    if self.old_value and self.old_value ~= self.value then
      if (self.changed) then
        self:changed()
      end
    end
    self.old_value = self.value
    self:needsredraw()
  end
  
  function wid:gotfocus()
    self.state = "input"
    self.backspace = false
    self:needsredraw()
  end
  
  function wid:update()
    if (self.state == "input") then
      
      -- special keys
      if (inp.iskeydown(inp.BACKSPACE)) then
        if not (self.backspace) then
          -- delete part of value if we have a range of value selected
          if (self.selection.finish) then
            self.value = string.sub(self.value, 1, self.selection.start) .. string.sub(self.value, self.selection.finish+1)
          else
            self.selection.start = self.selection.start - 1
            if (self.selection.start >= 0) then
              self.value = string.sub(self.value, 1, self.selection.start) .. string.sub(self.value, self.selection.start+2)
            end
          end
          self.selection.finish = nil
          
        end
        self.backspace = true
        self:needsredraw()
        
      elseif (inp.iskeydown(inp.DEL)) then
        if not (self.deletekey) then
          -- delete part of value if we have a range of value selected
          if (self.selection.finish) then
            self.value = string.sub(self.value, 1, self.selection.start) .. string.sub(self.value, self.selection.finish)
          else
            self.value = string.sub(self.value, 1, self.selection.start) .. string.sub(self.value, self.selection.start+2)
          end
          self.selection.finish = nil

        end
        self.deletekey = true
        self:needsredraw()
        
      elseif (inp.iskeydown(inp.LEFT)) then
        -- left arrow key
        if (inp.iskeydown(inp.LSHIFT)) then
          -- shift pushed = selection
          if (self.selection.finish == nil) then -- first selection
            self.selection.finish = self.selection.start
            self.selection.start = self.selection.start - 1
            self.selection.direction = -1
          else
            if (self.selection.direction == -1) then
              self.selection.start = self.selection.start - 1
            else
              self.selection.finish = self.selection.finish - 1
            end
          end
        else
          self.selection.finish = nil
          self.selection.start = self.selection.start - 1
        end
        
        self:needsredraw()
      elseif (inp.iskeydown(inp.RIGHT)) then
        -- right arrow key
        if (inp.iskeydown(inp.LSHIFT)) then
          -- shift pushed = selection
          if not (self.selection.finish) then -- first selection
            self.selection.finish = self.selection.start + 1
            self.selection.direction = 1
          else
            if (self.selection.direction == 1) then
              self.selection.finish = self.selection.finish + 1
            else
              self.selection.start = self.selection.start + 1
            end
          end
        else
          self.selection.finish = nil
          self.selection.start = self.selection.start + 1
        end
        
        self:needsredraw()
      else
        
        -- reset special keys
        self.backspace = false
        self.deletekey = false
        
        -- char inputs
        local c = inp.getlastchar()
        inp.clearlastchar()
      
        if not (c == 0) then
          
          -- delete part of value if we have a range of value selected
          if (self.selection.finish) then
            self.value = string.sub(self.value, 1, self.selection.start) .. string.char(c) .. string.sub(self.value, self.selection.finish+1)
          else          
            self.value = string.sub(self.value, 1, self.selection.start) .. string.char(c) .. string.sub(self.value, self.selection.start+1)
            self.selection.start = self.selection.start + 1
          end
          
          self.selection.finish = nil
          
        end
        
        self:needsredraw()
        
      end
      
      
      self:sanitycheck_selection()
      
    end
  end
  
  function wid:draw(force)
    if (self.redraw_needed or force) then
      gfx.translate(self.drawbox.x, self.drawbox.y)
      
      -- bg
      --gfx.drawtopleft(0, 0, self.drawbox.w, self.drawbox.h,
      --                40,6,1,1)
      
      -- bg
  		gfx.drawtopleft(1, 1, self.drawbox.w-2, self.drawbox.h-2, 2, 2, 1, 1)
  		
  		-- borders
  		gfx.drawtopleft(1,0,self.drawbox.w-2,1,1,5,1,1) -- top
  		gfx.drawtopleft(self.drawbox.w-1,1,1,self.drawbox.h-2,1,5,1,1) -- right
  		gfx.drawtopleft(1,self.drawbox.h-1,self.drawbox.w-2,1,1,5,1,1) -- bottom
  		gfx.drawtopleft(0,1,1,self.drawbox.h-2,1,5,1,1) -- right
      
      
      local oldtex = gfx.bindtexture(0)
                      
      local out_str = self.value
      
      -- find out what is visible
      out_str = string.sub(self.value, self.viewstart+1, self.viewstart+1+self.maxvisible)        
      
      -- password?
      if (self.masked) then
        local new_outstr = ""
        for i=1,#out_str do
          new_outstr = new_outstr .. self.masked
        end
        out_str = new_outstr
      end
        
      if not (self.selection.finish == nil) then
        -- selected range
        local r,g,b = gfx.getcolor()
        gfx.setcolor(0.8,0.5,0.5)
        local sx1 = self.stdpadding / 2 + (self.selection.start - self.viewstart)*8
        local sx2 = self.stdpadding / 2 + (self.selection.finish - self.viewstart)*8
        if (sx1 < self.stdpadding / 2) then
          sx1 = self.stdpadding / 2
        elseif (sx1 > self.drawbox.w - self.stdpadding / 2) then
          sx1 = self.drawbox.w - self.stdpadding / 2
        end
        if (sx2 < self.stdpadding / 2) then
          sx2 = self.stdpadding / 2
        elseif (sx2 > self.drawbox.w - self.stdpadding / 2) then
          sx2 = self.drawbox.w - self.stdpadding / 2
        end
        gfx.drawtopleft(sx1, self.drawbox.h / 2 - self.selectionheight / 2, sx2-sx1, self.selectionheight,
                        17,6,1,1)
        gfx.setcolor(r,g,b)
      end
      
      -- render visible string
      gui:drawfont(out_str, self.stdpadding / 2+4, self.drawbox.h / 2)
      
      if (self.state == "input" and not self.selection.finish) then
        local r,g,b = gfx.getcolor()
        gfx.setcolor(0.8,0.5,0.5)
        gui:drawfont("-", self.stdpadding / 2 + 4 + (self.selection.start - self.viewstart)*8, self.drawbox.h / 2+4);
        
        gfx.setcolor(r,g,b)
      end
      
      gfx.bindtexture(oldtex)
      gfx.translate(-self.drawbox.x, -self.drawbox.y)
      
    end
  end
  
  return wid
end



