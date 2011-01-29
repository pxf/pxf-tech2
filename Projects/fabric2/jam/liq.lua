require("jam/vecmath")

function create_new_liq(x,y,size, mass)
  local liq = {x = x, y = y, size = size, forces = {0,0}, mass = mass,
               vel = {0,0}}
  
  function liq:intersect(b)
    local tx = b.x - self.x
    local ty = b.y - self.y
    local dist = math.sqrt(tx*tx + ty*ty)
    
    if (dist - self.size - b.size <= 0) then
      local r = vec(tx,ty)
      r = r:norm()
      r = r*math.abs(dist - self.size - b.size)
      return r
    else
      return nil
    end
  end
  
  function liq:draw()
    gfx.bindtexture(0)
    gfx.drawcentered(self.x, self.y, self.size * 2, self.size * 2)
  end
  
  function liq:apply_force( x,y )
    self.forces[1] = self.forces[1] + x
    self.forces[2] = self.forces[2] + y
  end
  
  function liq:step(time)
    -- f = ma, a = f / m, v = s/t, s = v * t, v = a * t = (f / m) * t
    self.vel[1] = self.vel[1] + (self.forces[1] / self.mass) * time
    self.vel[2] = self.vel[2] + (self.forces[2] / self.mass) * time
    
    -- step pos
    self.x = self.x + self.vel[1] * time
    self.y = self.y + self.vel[2] * time
    
    -- reset forces
    self.forces = {0,0}
  end
  
  return liq
end