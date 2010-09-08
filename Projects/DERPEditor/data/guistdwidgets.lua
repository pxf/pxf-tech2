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
                      17,0,1,1)
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
    if (button == inp.MOUSE_RIGHT) then -- TODO: change this to MOUSE_MIDDLE
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
  
  --wid.superdraw = wid.draw
  function wid:draw(force)
    if (self.redraw_needed or force) then
      local r,g,b = gfx.getcolor()
      gfx.setcolor(46/256,46/256,46/256)
      gfx.drawtopleft(self.drawbox.x, self.drawbox.y, self.drawbox.w, self.drawbox.h,
                      17,0,1,1)
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
      for k,v in pairs(self.childwidgets) do
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
  end
  
  function wid:childisredrawn()
    --if not self.redraw_needed then
      self:needsredraw()
    --end
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
      gfx.drawtopleft(0, 0, self.drawbox.w, self.drawbox.h,17,1,1,1)
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


-- simple button aoeu
function gui:create_simplebutton(x,y,w,h,action)
  local wid = gui:create_basewidget(x,y,w,h)
  wid.action = action
  
  function wid:mouserelease(mx,my,button)
    if (button == inp.MOUSE_LEFT) then
      self:action(mx,my,button)
    end
  end
  
  function wid:draw(force)
    if (self.redraw_needed or force) then
      gfx.translate(self.drawbox.x, self.drawbox.y)
    
      -- bg
      gfx.drawtopleft(2, 2, self.drawbox.w-4, self.drawbox.h-4,
                      511,1,1,255)
                    
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
    
    
      gfx.translate(-self.drawbox.x, -self.drawbox.y)
    
    end
  end
  
  return wid
end

