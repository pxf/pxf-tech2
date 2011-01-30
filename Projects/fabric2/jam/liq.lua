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
    gfx.drawcentered(self.x, self.y, 32, 32)--self.size * 2, self.size * 2)
  end
  
  function liq:apply_force( x,y )
    self.forces[1] = self.forces[1] + x
    self.forces[2] = self.forces[2] + y
  end
  
  function liq:undo_step()
    self.x = self.old_x
    self.y = self.old_y
  end
  
  function liq:inverse_step()
    self.x = self.old_x
    self.y = self.old_y
    
    local inv = (vec(0,0) - vec(self.vel[1], self.vel[2])) * vec(0.9, 0.4)
    self.vel[1] = inv.x
    self.vel[2] = inv.y
  end
  
  function liq:step(time)
    self.old_x = self.x
    self.old_y = self.y
    
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


function create_liq_world()
  local liqworld = {liqs = {}, liq_tex = gfx.loadtexture(1024, "jam/liq.png", true)}
  
  function liqworld:add_liq(a)
    table.insert(self.liqs, a)
  end
  
  function liqworld:draw()
    local oldtex = gfx.bindtexture(self.liq_tex)
    --GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    --gfx.blending(gfx.SRC_ALPHA, gfx.ONE_MINUS_SRC_ALPHA)
    gfx.alphatest(gfx.GEQUAL, 0.5)
    for i=1,#self.liqs do
      self.liqs[i]:draw()
    end
    gfx.blending()
    gfx.bindtexture(oldtex)
  end
  
  function liqworld:step(time, df_world)
    
    -- apply gravity
    for i=1,#self.liqs do
      self.liqs[i]:apply_force(0, 9.82)
    end
    
    -- step each liq
    for i=1,#self.liqs do
      self.liqs[i]:step(time)
    end
    
    for i=1,#self.liqs do
      -- check if we collide with world
      if (df_world:hittest(math.floor(self.liqs[i].x), math.floor(self.liqs[i].y))) then
        --self.liqs[i]:undo_step()
        -- get response direction
        local old_x = math.floor(self.liqs[i].x)
        local old_y = math.floor(self.liqs[i].y)
        local res = df_world:get_response_vec(math.floor(self.liqs[i].x), math.floor(self.liqs[i].y))
        self.liqs[i]:inverse_step()
        
        local rel = vec(0,0)
        local new_x = math.floor(self.liqs[i].x)
        local new_y = math.floor(self.liqs[i].y)
        if (new_x < old_x) then
          rel.x = -1
        elseif (new_x > old_x) then
          rel.x = 1
        end
        if (new_y < old_y) then
          rel.y = -1
        elseif (new_y > old_y) then
          rel.y = 1
        end
        rel = rel:norm()
        
        -- create reflection force
        local refl_force = vec(0,0)
        if (res == "-") then
          if (rel.y < 0) then
            refl_force[2] = -1
          else
            refl_force[2] = 1
          end
          
        elseif (res == "|") then
          if (rel.x < 0) then
            refl_force[1] = -1
          else
            refl_force[1] = 1
          end
          
        elseif (res == "/") then
          if (rel.x < 0 and rel.y < 0) then
            refl_force[1] = -1
            refl_force[2] = -1
          else
            refl_force[1] = 1
            refl_force[2] = 1
          end
        elseif (res == "\\") then
          if (rel.x > 0 and rel.y > 0) then
            refl_force[1] = 1
            refl_force[2] = -1
          else
            refl_force[1] = -1
            refl_force[2] = 1
          end
        end
        
        -- normalize refl vect and multiply by amount intersection itwas
        --refl_force = vec(0,0) - refl_force
        refl_force = refl_force:norm() * vec(50, 50)
        self.liqs[i]:apply_force(refl_force[1], refl_force[2])
        
      end
    end
    
    -- perform intersection on each liq
    local amp = 0.5
    for i=1,#self.liqs do
      for j=i,#self.liqs do
        
      	local res = self.liqs[i]:intersect(self.liqs[j])
      	if (res ~= nil) then
      	  local nx = res.x
      	  local ny = res.y
      	  self.liqs[j]:apply_force(nx*amp,ny*amp)
      	  local res2 = vec(0,0)-res
      	  local nx = res2.x
      	  local ny = res2.y
      	  self.liqs[i]:apply_force(nx*amp,ny*amp)
        end
        
      end
    end
  end
  
  return liqworld
end

