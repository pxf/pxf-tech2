gui = {}
gui.snap_to_grid = false
gui.grid_size = 100

----------------------------------------------
-- Adding a comment, hey now.

----------------------------------------------
-- require window handling extension
require("fabric/guiwindows")

----------------------------------------------
-- base classes for widgets

function gui:create_basewidget(x,y,w,h)
  local wid = {
    hitbox = {x = x, y = y, w = w, h = h},
    drawbox = {x = x, y = y, w = w, h = h},
    parent = nil,
    redraw_needed = false,
    visible = true,
    widget_type = "stdwidget" -- stdwidget, menu
	
  }
  
  function wid:destroy()
    self:needsredraw()
    
    -- destroy childs
    for k,v in pairs(self.childwidgets) do
      v:destroy()
    end
    
    -- destroy self from parent
    if self.parent then
      --self.parent:removewidget(self)
      local deletek = nil
      for k,v in pairs(self.parent.childwidgets) do
        if v == self then
          deletek = k
          break
        end
      end
      
      if deletek then
        self.parent.childwidgets[deletek] = nil
      end
    end
  end
  
  -- child widget control
  wid.childwidgets = {}
  function wid:addwidget(cwid,key)
    cwid.parent = self
	
	gui:set_focus(cwid)
    cwid:needsredraw()
	
  	if key then
  		self.childwidgets[key] = cwid
  	else
  		table.insert(self.childwidgets, cwid)
  	end
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
			self.childwidgets[find_k] = nil
		end
	end
  
  -----------------------------------
  -- update functions
  --  (can be useful for animation or input)
  function wid:update()
    for k,v in pairs(self.childwidgets) do
		  v:update(force)
		end
  end
  
  -----------------------------------
  -- store key shortcuts in a table
  wid.shortcuts = {} -- { { keys = {inp.LSHIFT, 'C'}, onpress = function () print("LOL SUP") end} }
  
  -----------------------------------
  -- redraw functions
  function wid:needsredraw(full)
    if (full) then
      gui:redraw()
    else
      local x,y = self:find_abspos(self)
      gui:redraw(x, y, self.drawbox.w, self.drawbox.h)
    end
    self.redraw_needed = true
    
    -- notify parent
    if not (self.parent == nil) then
      self.parent:childisredrawn()
    end
  end
  function wid:childisredrawn()
    -- do nothing as standard
  end
  function wid:resetredraw()
    self.redraw_needed = false
    for k,v in pairs(self.childwidgets) do
      v:resetredraw()
    end
  end
  function wid:find_abspos(sender)
    local x,y
    x = self.drawbox.x
    y = self.drawbox.y
    
    if not (self.parent == nil) then
      local tx,ty = self.parent:find_abspos(sender)
      x = x + tx
      y = y + ty
    end
    
    return x,y
  end
  function wid:move_relative(x,y)
    self.hitbox.x = self.hitbox.x + x
    self.hitbox.y = self.hitbox.y + y
    self.drawbox.x = self.drawbox.x + x
    self.drawbox.y = self.drawbox.y + y
  end
  function wid:move_abs(x,y)
    self.hitbox.x = x
    self.hitbox.y = y
    self.drawbox.x = x
    self.drawbox.y = y
  end
  function wid:resize_relative(w,h)
    self.hitbox.w = self.hitbox.w + w
    self.hitbox.h = self.hitbox.h + h
    self.drawbox.w = self.drawbox.w + w
    self.drawbox.h = self.drawbox.h + h
    
    -- notify parent
    if not (self.parent == nil) then
      self.parent:child_resized(self)
    end
  end
  function wid:resize_abs(w,h)
    self.hitbox.w = w
    self.hitbox.h = h
    self.drawbox.w = w
    self.drawbox.h = h
    -- notify parent
    if not (self.parent == nil) then
      self.parent:child_resized(self)
    end
  end
  function wid:child_resized(cwid)
    -- do nothing ?

  end
  
  function wid:resize_callback(w,h,edge)
  	for k,v in pairs(self.childwidgets) do
  		v:resize_callback(w,h)
  	end
  end
  
  
  
  -- end of redraw functions
  ----------------------------------
  
  function wid:draw(force)
  	if (self.redraw_needed or force) and self.visible then
		
  		if (gui.draw_debug_rects) then
  			gfx.drawtopleft(self.drawbox.x, self.drawbox.y, self.drawbox.w, 1, 5, 5, 1, 1) -- top
  			gfx.drawtopleft(self.drawbox.x, self.drawbox.y, 1, self.drawbox.h, 5, 5, 1, 1) -- left
  			gfx.drawtopleft(self.drawbox.x + self.drawbox.w, self.drawbox.y, 1,self.drawbox.h, 5, 5, 1, 1) -- right
  			gfx.drawtopleft(self.drawbox.x, self.drawbox.y + self.drawbox.h, self.drawbox.w, 1, 5, 5, 1, 1) -- bottom
  		end
		
		if (gui.draw_hitbox_rects) then
			gfx.drawtopleft(self.hitbox.x, self.hitbox.y, self.hitbox.w, 1, 17, 5, 1, 1) -- top
  			gfx.drawtopleft(self.hitbox.x, self.hitbox.y, 1, self.hitbox.h, 17, 5, 1, 1) -- left
  			gfx.drawtopleft(self.hitbox.x + self.hitbox.w, self.hitbox.y, 1,self.hitbox.h, 17, 5, 1, 1) -- right
  			gfx.drawtopleft(self.hitbox.x, self.hitbox.y + self.hitbox.h, self.hitbox.w, 1, 17, 5, 1, 1) -- bottom
		end
		
  		gfx.translate(self.drawbox.x, self.drawbox.y)
  		for k,v in pairs(self.childwidgets) do
  		  v:draw(force)
  		end
  		gfx.translate(-self.drawbox.x, -self.drawbox.y)
  	end
  end
  
  function wid:hittest(x0,y0,x1,y1)
  	if not self.visible then
  		return false
  	end
	
    if (x1 < self.hitbox.x) then
      return false
    elseif (x0 > self.hitbox.x + self.hitbox.w) then
      return false
    elseif (y1 < self.hitbox.y) then
      return false
    elseif (y0 > self.hitbox.y + self.hitbox.h) then
      return false
    end
      
    return true
  end
  
  function wid:hittest_d(x0,y0,x1,y1) -- drawbox hittest
   	if not self.visible then
  		return false
  	end
	
    if (x1 < self.drawbox.x) then
      return false
    elseif (x0 > self.drawbox.x + self.drawbox.w) then
      return false
    elseif (y1 < self.drawbox.y) then
      return false
    elseif (y0 > self.drawbox.y + self.drawbox.h) then
      return false
    end
      
    return true
  end
  
  function wid:find_mousehit(mx,my)
    if (self:hittest(mx,my,mx,my)) then
      local thit = nil
      for k,v in pairs(self.childwidgets) do
        if v then
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
      
      return self
    end
    
    return nil
  end
  
  function wid:find_redrawhit(rx0,ry0,rx1,ry1)
    -- should we redraw?
    if (self:hittest_d(rx0,ry0,rx1,ry1)) then
      self.redraw_needed = true
    end
    
    -- should any of our childs redraw?    
    for k,v in pairs(self.childwidgets) do
      v:find_redrawhit(rx0 - self.drawbox.x, ry0 - self.drawbox.y, rx1 - self.drawbox.x, ry1 - self.drawbox.y)
    end
  end
  
  function wid:mouseleave(mx,my)
	--print(self.widget_type)
  end
  
  function wid:mouseover(mx,my)
  
  end
  
  return wid
end

function gui:create_root()
  local rootwid = self:create_basewidget(0, 0, app.width, app.height)
  
  -- child widget control
  rootwid.super_addwidget = rootwid.addwidget
  function rootwid:addwidget(cwid)
    self:super_addwidget(cwid)
    gui:set_focus(cwid)
  end
  
  function rootwid:draw(force)
    --[[local r,g,b = gfx.getcolor()
    local oldtex = gfx.bindtexture(0)
    gfx.setcolor(5/255,5/255,5/255)
    gfx.drawtopleft(0, 0, self.drawbox.w, self.drawbox.h)
    gfx.setcolor(r,g,b)
    gfx.bindtexture(oldtex)]]
    
    for k,v in pairs(self.childwidgets) do
      gfx.translate(self.drawbox.x, self.drawbox.y)
      v:draw(force)
      gfx.translate(-self.drawbox.x, -self.drawbox.y)
    end
    
  end
  
  function rootwid:find_mousehit(mx,my)
    if (self:hittest(mx,my,mx,my)) then
      local thit = nil
      for k,v in pairs(self.childwidgets) do
        if v then
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
      
      return nil -- always return nil for root object, since the root does not have any "input" (ie. click through it)
    end
    
    return nil
  end
  
  return rootwid
end

----------------------------------------------
-- core and setup of GUI

gui.redrawrects = {}
gui.tooltip = {body = nil, timeout = 0, x = 0, y = 0}
function gui:redraw(x,y,w,h)
  if x == nil then
    gfx.redrawneeded()
  else
    self.widgets:find_redrawhit(x,y,x+w,y+h)
    gfx.redrawneeded(x,y,w,h)
    --print("redraw area: " .. tostring(x) .." " .. tostring(y) .. " " .. tostring(w) .." " .. tostring(h))
    table.insert(gui.redrawrects, 1, {x,y,w,h})
  end
  --gfx.redrawneeded()
end

function gui:set_tooltip(body, x, y)
  self.tooltip.body = body
  self.tooltip.timeout = 50
  self.tooltip.x = x + 12
  self.tooltip.y = y - 32
end

function gui:set_focus(wid)
  if (self.focuswidget) then
    if not (self.focuswidget == wid) then
      if (self.focuswidget.lostfocus) then
        self.focuswidget:lostfocus(wid)
      end
    end
  end
  self.focuswidget = wid
  
  if (self.focuswidget) then
    if (self.focuswidget.gotfocus) then
      self.focuswidget:gotfocus()
    end
  end
  
end

function gui:drawrightfont(str,x,y)
  local len = gui:get_font_length(str)
  local x2 = x - ((len-1) * 8)
  local y2 = y
  gui:drawfont(str, x2, y2)
end

function gui:drawcenteredfont(str,x,y)
  local len = gui:get_font_length(str)
  local x2 = x - ((len-1) * 8) / 2
  local y2 = y + (4 / 2) - 1
  gui:drawfont(str, x2, y2)
end

function gui:get_font_length(str)
  local len = #string.gsub(str, "%^%(.-%){(.-)}", "%1")
  return len
end

function gui:drawfont(str,x,y)
  local oldtex = gfx.bindtexture(self.font)
  --local r,g,b = gfx.getcolor()
  --gfx.setcolor(1, 1, 1)
  gfx.translate(x, y)
	local strlen = #str
	local char_w = 8
	local line_h = 16
	
	-- newline stuff
	local start_x = x
	local current_x = 0
	local current_y = 0
	
	-- color stuff
	local char_skip = 0
	local color_old = {1,1,1}
	local color_length = -1
	
	local char_counter = 0
	local euro_next = false
	
	for i=1,strlen do
	  if (char_skip > 0) then
	    char_skip = char_skip - 1
    else
      if (color_length > 0) then
        color_length = color_length - 1
      end
      
      if (color_length == 0) then
        -- reset old color
        gfx.setcolor(color_old[1],color_old[2],color_old[3])
        color_length = -1
      else      
    	  -- calculate tex coords
    	  local index = string.byte(str, i)
    	  if (index == 195) then
    	    -- found special char
    	    euro_next = true
    	  elseif (index == 10) then
    	    -- newline
    	    current_x = start_x
    	    current_y = current_y + line_h
    	  elseif (index == 94) then
    	    -- color change
  	      local start,j,r,g,b,s = string.find(string.sub(str, i), "%((.-),(.-),(.-)%){(.-)}") 
  	      if start then
  	        color_length = #s+1
  	        color_old[1],color_old[2],color_old[3] = gfx.getcolor()
  	        gfx.setcolor(tonumber(r), tonumber(g), tonumber(b))
	        
  	        -- find out how many chars to skip
    	      char_skip = #("(" .. r .. "," .. g .. "," .. b .. "){")
          end
        else	  
      	    -- draw quad
      	    if (euro_next) then
      	      euro_next = false
      	      index = index + 32
    	      else
    	        index = index - 32
    	      end
        	  local s = math.fmod(index, 16) * 16
        	  local t = math.floor(index / 16) * 16
      	    gfx.drawcentered(current_x, current_y, 16, 16, s, t, 16, 16)
      	    current_x = current_x + char_w
      	    char_counter = char_counter + 1
    
        end
      end
    end
	end
	
	gfx.translate(-x, -y)
	--gfx.setcolor(r,g,b)
	gfx.bindtexture(oldtex)
  
end

function gui:toggle_show_redraw()
  self.draw_redraw_rects = not self.draw_redraw_rects
  print("draw_redraw_rects: " .. tostring(self.draw_redraw_rects))
  return (not self.draw_redraw_rects)
end

function gui:statusbarinfo(str)
  if (self.statusbar) then
    self.statusbar:settext(str)
  end
end

function gui:draw_custom_cursor(force)
	if (self.needsredraw or force) and not self.mousevisible then
		local c = self.current_cursor
		local mx,my = inp.getmousepos()
			
		if c then	
			--gui:showmouse(false)
			
			local old_tex = gfx.bindtexture(self.themetex)
		
			gfx.translate(mx,my)
			gfx.drawtopleft(0,0,c.w,c.h,c.s,c.t,c.w,c.h)
			gfx.translate(-mx,-my)
			
			gfx.bindtexture(old_tex)	
		end
	end
end

function gui:set_cursor(name)
	for k,v in pairs(self.custom_cursors) do
		if v.name == name then
			self.current_cursor = v
		end
	end
end

function gui:add_customcursor(w,h,s,t,name)
	local cursor = { w = w, h = h, s = s, t = t, name = name}
	
	table.insert(self.custom_cursors,cursor)
	
	return cursor
end

function gui:init()
  self.themetex = gfx.loadtexture(2048*2, "fabric/guitheme_brown.png")
  self.font = gfx.loadtexture(1024*4, "fabric/charmap_monaco_shadow.png", true)
  self.mouse = {pushed = false, buttonid = nil, lastpos = {x=0,y=0}}
  
  self.use_customcursor = true
  self.current_cursor = nil
  
  self.custom_cursors = { }
  
  self.activewidget = nil
  self.focuswidget = nil
  self.highlight_widget = nil
  
  self.draw_debug_rects = false
  self.draw_redraw_rects = false
  self.draw_hitbox_rects = false
  
  -- statusbar widget
  self.statusbar = nil
  
  -- tree of widgets
  self.widgets = gui:create_root()
 
end

function gui:showmouse(toggle)
  if (toggle == nil) then
    if (self.mousevisible) then
      self.mousevisible = false
    else
      self.mousevisible = true
    end
    toggle = self.mousevisible
  end
  
  self.mousevisible = toggle
  inp.showmouse(self.mousevisible)
end

function gui:update()
  gui.redrawrects = {}
  -- this should be called each app update
  local mx,my = inp.getmousepos()
  
  -- reset redraws
  self.widgets:resetredraw()
  
  -- test
  --gui:redraw(mx,my,32,32)
  
  -- check keyboard shortcuts for the focused widget
  if (self.focuswidget) then
	local done = false
    for _,shortcut in pairs(self.focuswidget.shortcuts) do
      local shortcut_success = true
	  local release = false
	  local mouse_release = false
	  
		if shortcut.mouse then
		  for __,button in pairs(shortcut.mouse) do
			if not (inp.isbuttondown(button)) then
				if shortcut.was_pressed then
					mouse_release = true
				end
				
				shortcut_success = false
				break
			end
		  end
		else
			mouse_release = true
		end
		
	  if shortcut.keys then
		  for __,key in pairs(shortcut.keys) do
			if not (inp.iskeydown(key)) then
			  if shortcut.was_pressed then
				release = true
			  end
			  
			  shortcut_success = false
			  break
			end
		  end
	  else
		release = true
	  end
	
		release = mouse_release and release
		
      
      if (shortcut_success and (not shortcut.was_pressed) ) then
		shortcut.was_pressed = true
        if shortcut.onpress then 
			shortcut:onpress()
		end
      elseif (release) then
		shortcut.was_pressed = false
		
		if shortcut.onrelease then
			shortcut:onrelease()
		end
	  end
	  
    end
  end
  
  -- send mouse over
  local mouse_over_rcv = self.widgets:find_mousehit(mx,my)
  
  --print("type: " .. mouse_over_rcv.widget_type)
  
  if mouse_over_rcv then
    mouse_over_rcv:mouseover(mx,my)
	
	if self.highlight_widget == nil then
		self.highlight_widget = mouse_over_rcv
	elseif not (self.highlight_widget == mouse_over_rcv) then
		self.highlight_widget:mouseleave(mx,my)
		self.highlight_widget = mouse_over_rcv
	end
  end
		
  
  -- mouse operations on widgets
  if (inp.isbuttondown(inp.MOUSE_LEFT) or
      inp.isbuttondown(inp.MOUSE_RIGHT) or
      inp.isbuttondown(inp.MOUSE_MIDDLE)) then
    
    -- different button than before?
    if (self.buttonid == nil) then
      if inp.isbuttondown(inp.MOUSE_LEFT) then
        self.buttonid = inp.MOUSE_LEFT
      elseif inp.isbuttondown(inp.MOUSE_RIGHT) then
        self.buttonid = inp.MOUSE_RIGHT
      else
        self.buttonid = inp.MOUSE_MIDDLE
      end
    end
    
    -- if we weren't pushing before, find new active widget
    if (not self.mouse.pushed) then
      --self.activewidget
      self.activewidget = self.widgets:find_mousehit(mx,my)
      --print("new active widget: " .. tostring(self.activewidget) .. " (has type " .. self.activewidget.widget_type .. ")")
      
      -- active widget is now the focus widget
      self:set_focus(self.activewidget)
      
      --if not (self.activewidget == self.widgets) then
        __guihit = true
      --end
      
      if (self.activewidget and self.activewidget.mousepush) then
        self.activewidget:mousepush(mx,my,self.buttonid)
      end
      
      
    else
      -- we might have a drag operation on our hands!
      
      if (self.activewidget and self.activewidget.mousedrag) then
        self.activewidget:mousedrag(mx-self.mouse.lastpos.x, my-self.mouse.lastpos.y,self.buttonid)
      end
    end
    
    self.mouse.lastpos.x = mx
    self.mouse.lastpos.y = my
    self.mouse.pushed = true
  else
    -- if mouse button was released
    if (self.mouse.pushed) then
      
      if (self.activewidget) then
        if (self.activewidget.mouserelease) then
          self.activewidget:mouserelease(mx,my,self.buttonid)
        end
      end
      
      self.activewidget = nil
      -- TODO: No more active widget... aoeaoe
      
      self.mouse.pushed = false
      self.buttonid = nil
    end
    
  end
  
  -- call update function on widgets
  self.widgets:update()
  
  -- update tooltip
  if (self.tooltip.timeout > 0) then
    self.tooltip.timeout = self.tooltip.timeout - 1
  end
  
  if (self.activewidget) then
    __guihit = true
  end
  
  --[[if (inp.isbuttondown(inp.MOUSE_LEFT)) then
    -- check if we hit something
    local mx,my = inp.getmousepos()
    for k,v in pairs(self.widgets) do
      if (v:hittest(mx,my,mx,my)) then
        -- TODO: follow down the tree
        print("hit: '" .. tostring(k) .. "'")
        
      end
    end
  end]]
end

function gui:draw(force)
  -- redraw gui
  -- NOTE: for efficiency, redrawneeded(...) should be called beforehand
  --       to minimize fill
  
  local oldtex = gfx.bindtexture(self.themetex)
  self.widgets:draw(force)
  
  -- Draw tooltip
  if (self.tooltip.timeout > 0) then
    gfx.translate(self.tooltip.x,self.tooltip.y)
    if (type(self.tooltip.body) == "string") then
      local w = #self.tooltip.body*8 + 20
      gfx.drawtopleft(1,1,w-2, 18, 5,1,1,1)
      gfx.drawtopleft(1,0,w-2, 1, 1,5,1,1) -- top
      gfx.drawtopleft(1,19,w-2, 1, 1,5,1,1) -- bottom
      gfx.drawtopleft(0,1,1, 18, 1,5,1,1) -- left
      gfx.drawtopleft(w-1,1,1, 18, 1,5,1,1) -- right
      
      local r,g,b = gfx.getcolor()
      gfx.setcolor(224/255,126/255,0)
      gui:drawfont(self.tooltip.body, 15, 10)
      gfx.setcolor(r,g,b)
    else
      self.tooltip.body:draw(true)
    end
    gfx.translate(-self.tooltip.x,-self.tooltip.y)
  end
  
  -- show redraw regions
  if (self.draw_redraw_rects) then
    for k,v in pairs(gui.redrawrects) do
      local r,g,b = gfx.getcolor()
      gfx.setcolor((k % 3) *30, (k % 2) *30, (k % 1) *30)
      gfx.drawtopleft(v[1], v[2], v[3], 1, 17, 1, 1, 1) -- top
      gfx.drawtopleft(v[1], v[2] + v[4]-1, v[3], 1, 17, 1, 1, 1) -- top
      gfx.drawtopleft(v[1] + v[3]-1, v[2], 1, v[4], 17, 1, 1, 1) -- right
      gfx.drawtopleft(v[1], v[2], 1, v[4], 17, 1, 1, 1) -- left
      gfx.setcolor(r,g,b)
    end
  end
  
  gfx.bindtexture(oldtex)
end