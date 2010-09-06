gui = {}

----------------------------------------------
-- base classes for widgets

function gui:create_basewidget(x,y,w,h)
  local wid = {
    hitbox = {x = x, y = y, w = w, h = h, parent = nil}
  }
  
  -- child widget control
  wid.childwidgets = {}
  function wid:addwidget(cwid)
    cwid.parent = self
    table.insert(self.childwidgets, cwid)
  end
  
  function wid:draw()
    for k,v in pairs(self.childwidgets) do
      v:draw()
    end
  end
  
  function wid:hittest(x0,y0,x1,y1)
    if (x0 < self.hitbox.x) then
      return false
    elseif (x1 > self.hitbox.x + self.hitbox.w) then
      return false
    elseif (y0 < self.hitbox.y) then
      return false
    elseif (y1 > self.hitbox.y + self.hitbox.h) then
      return false
    end
      
    return true
  end
  
  function wid:find_mousehit(mx,my)
    if (self:hittest(mx,my,mx,my)) then
      local thit = nil
      for k,v in pairs(self.childwidgets) do
        thit = v:find_mousehit(mx,my)
        
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

function gui:create_root()
  local rootwid = self:create_basewidget(0, 0, app.width, app.height)
  
  return rootwid
end

function gui:create_testwidget(x,y,w,h)
  local wid = self:create_basewidget(x,y,w,h)
  
  function wid:draw()
    gfx.setcolor(1, 0, 0)
    gfx.drawcentered(self.hitbox.x + self.hitbox.w / 2, self.hitbox.y + self.hitbox.h / 2,
                     self.hitbox.w, self.hitbox.h)
  end
  
  function wid:mousepush(mx,my)
    
  end
  
  function wid:mouserelease(mx,my)
    print("clicked on this awesome widget")
  end
  
  function wid:mousedrag(dx,dy)
    if (not (dx == 0)) or (not (dy == 0)) then
      print("draging!! " .. tostring(dx) .. " " .. tostring(dy) )
    end
  end
  
  return wid
end

----------------------------------------------
-- core and setup of GUI

function gui:init()
  self.themetex = gfx.loadtexture("data/consolefont.png")
  self.mouse = {pushed = false, lastpos = {x=0,y=0}}
  
  self.activewidget = nil
  
  -- tree of widgets
  self.widgets = gui:create_root()
  self.widgets:addwidget(gui:create_testwidget(300,300,300,400))
end

function gui:update()
  -- this should be called each app update
  
  local mx,my = inp.getmousepos()
  if (inp.isbuttondown(inp.MOUSE_LEFT)) then
    
    -- if we weren't pushing before, find new active widget
    if (not self.mouse.pushed) then
      --self.activewidget
      self.activewidget = self.widgets:find_mousehit(mx,my)
      
      if (self.activewidget.mousepush) then
        self.activewidget:mousepush(mx,my)
      end
      
      print("new active widget: " .. tostring(self.activewidget))
    else
      -- we might have a drag operation on our hands!
      
      if (self.activewidget.mousedrag) then
        self.activewidget:mousedrag(mx-self.mouse.lastpos.x, my-self.mouse.lastpos.y)
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
          self.activewidget:mouserelease(mx,my)
        end
      end
      
      self.activewidget = nil
      -- TODO: No more active widget... aoeaoe
      
      self.mouse.pushed = false
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

function gui:draw()
  -- redraw gui
  -- NOTE: for efficiency, redrawneeded(...) should be called beforehand
  --       to minimize fill
  
  gfx.bindtexture(self.themetex)
  self.widgets:draw()
end