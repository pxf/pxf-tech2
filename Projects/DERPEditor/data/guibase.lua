gui = {}

----------------------------------------------
-- base classes for widgets

function gui:create_basewidget(x,y,w,h)
  local wid = {
    hitbox = {x = x, y = y, w = w, h = h},
    drawbox = {x = x, y = y, w = w, h = h},
    parent = nil,
    redraw_needed = false
  }
  
  function wid:destroy()
    if self.parent then
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
  function wid:addwidget(cwid)
    cwid.parent = self
    table.insert(self.childwidgets, cwid)
  end
  
  -----------------------------------
  -- redraw functions
  function wid:needsredraw()
    local x,y = self:find_abspos()
    gui:redraw(x, y, self.drawbox.w, self.drawbox.h)
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
  function wid:find_abspos()
    local x,y
    x = self.drawbox.x
    y = self.drawbox.y
    
    if not (self.parent == nil) then
      local tx,ty = self.parent:find_abspos()
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
  -- end of redraw functions
  ----------------------------------
  
  function wid:draw(force)
    gfx.translate(self.drawbox.x, self.drawbox.y)
    for k,v in pairs(self.childwidgets) do
      v:draw(force)
    end
    gfx.translate(-self.drawbox.x, -self.drawbox.y)
  end
  
  function wid:hittest(x0,y0,x1,y1)
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
        thit = v:find_mousehit(mx - self.hitbox.x, my - self.hitbox.y)
        
        if not (thit == nil) then
          -- we hit a child widget, return this one instead
          return thit
        end
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
  
  return wid
end

function gui:create_root()
  local rootwid = self:create_basewidget(0, 0, app.width, app.height)
  
  function rootwid:draw(force)
    local r,g,b = gfx.getcolor()
    gfx.setcolor(86/255,86/255,86/255)
    gfx.drawtopleft(0, 0, self.drawbox.w, self.drawbox.h,18,2,1,1)
    gfx.setcolor(r,g,b)
    
    gfx.translate(self.drawbox.x, self.drawbox.y)
    for k,v in pairs(self.childwidgets) do
      v:draw(force)
    end
    gfx.translate(-self.drawbox.x, -self.drawbox.y)
  end
  
  return rootwid
end

----------------------------------------------
-- core and setup of GUI

gui.redrawrects = {}
function gui:redraw(x,y,w,h)
  --self.widgets:find_redrawhit(x,y,x+w,y+h)
  gfx.redrawneeded(x,y,w,h)
  --print("redraw area: " .. tostring(x) .." " .. tostring(y) .. " " .. tostring(w) .." " .. tostring(h))
  table.insert(gui.redrawrects, 1, {x,y,w,h})
  --gfx.redrawneeded()
end

function gui:init()
  self.themetex = gfx.loadtexture("data/guitheme.png")
  self.mouse = {pushed = false, buttonid = nil, lastpos = {x=0,y=0}}
  
  self.activewidget = nil
  
  -- tree of widgets
  self.widgets = gui:create_root()
  --self.widgets:addwidget(gui:create_testwidget(300,300,300,400))
end

function gui:update()
  gui.redrawrects = {}
  -- this should be called each app update
  local mx,my = inp.getmousepos()
  
  -- reset redraws
  self.widgets:resetredraw()
  
  -- test
  --gui:redraw(mx,my,32,32)
  
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
      
      if (self.activewidget.mousepush) then
        self.activewidget:mousepush(mx,my,self.buttonid)
      end
      
      print("new active widget: " .. tostring(self.activewidget))
    else
      -- we might have a drag operation on our hands!
      
      if (self.activewidget.mousedrag) then
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

function gui:draw(force,show_redraw)
  -- redraw gui
  -- NOTE: for efficiency, redrawneeded(...) should be called beforehand
  --       to minimize fill
  
  local oldtex = gfx.bindtexture(self.themetex)
  self.widgets:draw(force)
  
  -- show redraw regions
  if (show_redraw) then
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