function gui:create_panel(x,y,w,h)
  local wid = gui:create_basewidget(x,y,w,h)
  
  function wid:mousedrag(dx,dy,button)
    if (button == inp.MOUSE_MIDDLE) then
      self.drawbox.x = self.drawbox.x + dx
      self.drawbox.y = self.drawbox.y + dy
      self.hitbox.x = self.hitbox.x + dx
      self.hitbox.y = self.hitbox.y + dy
    end
  end
  
  wid.superdraw = wid.draw
  function wid:draw()
    local r,g,b = gfx.getcolor()
    gfx.setcolor(46/256,46/256,46/256)
    gfx.drawtopleft(self.drawbox.x, self.drawbox.y, self.drawbox.w, self.drawbox.h,
                    17,0,1,1)
    gfx.setcolor(r,g,b)
    self:superdraw()
  end
    
  return wid
end

function gui:create_horisontalpanel(x,y,w,h)
  local wid = gui:create_basewidget(x,y,w,h)
  
  function wid:mousedrag(dx,dy,button)
    if (button == inp.MOUSE_MIDDLE) then
      self.drawbox.x = self.drawbox.x + dx
      self.hitbox.x = self.hitbox.x + dx
    end
  end
  
  wid.superdraw = wid.draw
  function wid:draw()
    local r,g,b = gfx.getcolor()
    gfx.setcolor(46/256,46/256,46/256)
    gfx.drawtopleft(self.drawbox.x, self.drawbox.y, self.drawbox.w, self.drawbox.h,
                    17,0,1,1)
    gfx.setcolor(r,g,b)
    self:superdraw()
  end
    
  return wid
end




function gui:create_simplebuttons(x,y,w,h,action)
  local wid = gui:create_basewidget(x,y,w,h)
  wid.action = action
  
  function wid:mouserelease(mx,my,button)
    if (button == inp.MOUSE_LEFT) then
      self:action(mx,my,button)
    end
  end
  
  function wid:draw()
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
  
  return wid
end

