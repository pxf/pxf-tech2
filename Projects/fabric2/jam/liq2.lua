require("jam/vecmath")

function create_liq(x,y, size, mass, liqtype, life_time)
  local liq = {x = x, y = y, radius = size,
               mass = mass,
               forces = {0,0},
               vel = {0,0},
               liqtype = liqtype}
			   
	liq.elapsed_time = 0.0
	liq.life_time = life_time
	liq.cell_index = nil
	
	function liq:undo_step()
    self.x = self.old_x
    self.y = self.old_y
  end
	
	function liq:step(time)
    self.old_x = self.x
    self.old_y = self.y
    
    -- f = ma, a = f / m, v = s/t, s = v * t, v = a * t = (f / m) * t
    self.vel[1] = self.vel[1] + (self.forces[1] / self.mass) * time
    self.vel[2] = self.vel[2] + (self.forces[2] / self.mass) * time
    
    local max_vel = 8
    local dir_vel = vec(self.vel[1], self.vel[2])
    local cur_vel = dir_vel:len()
    dir_vel = dir_vel:norm()
    if (cur_vel > max_vel) then
      cur_vel = max_vel
    end
    
    --self.vel[1] = dir_vel[1] * cur_vel
    --self.vel[2] = dir_vel[2] * cur_vel
    
    -- step pos
    self.x = self.x + self.vel[1] * time
    self.y = self.y + self.vel[2] * time
    
    -- reset forces
    self.forces = {0,0}
  end
	
	return liq
end

function create_liqworld(w,h)
  local liqworld = {w = w, h = h,
                    liqs = {},
                    grid = new_grid(w,h,32,32),
                    tex = gfx.loadtexture(512, "jam/circle.png", true)}
     
    
  -- functions that we apply on liqs               
  function liqworld:add_liq(a)
    table.insert(self.liqs, a)
    self.grid:insert(a)
  end
  
  function liqworld:apply_force(a, x,y )
    a.forces[1] = a.forces[1] + x
    a.forces[2] = a.forces[2] + y
  end
  
  function liqworld:intersect(a,b)
    local tx = b.x - a.x
    local ty = b.y - a.y
    local dist = math.sqrt(tx*tx + ty*ty)
    
    if (dist - (a.radius + b.radius) <= 0) then
      local r = vec(tx,ty)
      r = r:norm()
      r = r*math.abs(dist - (a.radius + b.radius))
      r[1] = r[1] / dist
      r[2] = r[2] / dist
      return r
    else
      return nil
    end
    
    --[[local tx = b.x - self.x
    local ty = b.y - self.y
    local dist = math.sqrt(tx*tx + ty*ty)
    
    if (dist - self.size - b.size <= 0) then
      local r = vec(tx,ty)
      r = r:norm()
      r = r*math.abs(dist - self.size - b.size)
      return r
    else
      return nil
    end]]
  end
  
  function liqworld:draw()
    local oldtex = gfx.bindtexture(self.tex)
    for k,v in pairs(self.liqs) do
      
      gfx.drawcentered(v.x, v.y, v.radius*2, v.radius*2)
      
    end
    
    gfx.bindtexture(oldtex)
    --gfx.bindtexture(0)
    --[[if (math.abs(self.vel[2]) > 1) then
		
      gfx.drawcentered(self.x-self.vel[1]*1.5, self.y-self.vel[2]*1.5, 12, 12)
      gfx.drawcentered(self.x-self.vel[1], self.y-self.vel[2], 24, 24)
    end
    gfx.drawcentered(self.x, self.y, 32, 32)--self.size * 2, self.size * 2)
    ]]
  end
  
  --[[function liq:undo_step()
    self.x = self.old_x
    self.y = self.old_y
  end]]
  
  --[[function liqworld:inverse_step()
    self.x = self.old_x
    self.y = self.old_y
    
    local inv = (vec(0,0) - vec(self.vel[1], self.vel[2])) * vec(0.8, 0.4)
    self.vel[1] = inv.x
    self.vel[2] = inv.y
  end]]
  
  function liqworld:step(time)
    
    -- step each liq
  	for k,v in pairs(self.liqs) do
  	 v:step(time)
  	end
    
    -- apply gravity
    for k,v in pairs(self.liqs) do
  		self:apply_force(v, 0, 0.4)--0.982)
		end
		
		--for k,v in pairs(self.liqs) do
		local amp = 0.9--4
		for a=1,#self.liqs do
		  for b=a+1,#self.liqs do
		    local a = self.liqs[a]
		    local b = self.liqs[b]
		    
  		  local res = self:intersect(a,b)

  			if (res ~= nil) then
  				local nx = res.x
  				local ny = res.y
  				
  				--local res2 = res:norm()
  				--local nx = res2.x
  				--local ny = res2.y
  				
  				-- reposition
  				--b.x = b.x - nx/2
  				--b.y = b.y - ny/2
  				
  				-- apply force
  				self:apply_force(b, nx*amp/2,ny*amp/2)
  				
  				b.vel[1] = b.vel[1] * 0.8
          b.vel[2] = b.vel[2] * 0.8
          --b:undo_step()
  				
  				-- reposition
  				--a.x = a.x + nx/2
  				--a.y = a.y + ny/2
  				
  				-- apply force
  				self:apply_force(a, -nx*amp/2,-ny*amp/2)		
  				
          a.vel[1] = a.vel[1] * 0.8
          a.vel[2] = a.vel[2] * 0.8
          --a:undo_step()
  			end
  			
  		end
	  end
		
		--[[
		local grid = self.grid
  	local amp = 1.0
  	for k,v in pairs(self.liqs) do
  		local i = v.cell_index

  		-- calc new index

  		local x = v.x
  		local y = v.y

  		local c_index = math.floor(x / grid.step_x)
  		local r_index = math.floor(y / grid.step_y)

  		local index = r_index * grid.ch + c_index 

  		if not (i == index) then
  			v.cell_index = index
  		end

  		index = v.cell_index

  		local candidates = {}

  		-- 0: top left
  		local index0 = index - grid.cw - 1 

  		--print(index,index0)

  		grid:cell_collision(candidates,v,index0)

  		-- 1: top middle
  		local index1 = index0 + 1
  		grid:cell_collision(candidates,v,index1)

  		-- 2: top right
  		local index2 = index1 + 1
  		grid:cell_collision(candidates,v,index2)

  		-- 3: mid left
  		index0 = index - 1
  		grid:cell_collision(candidates,v,index0)

  		-- 4: mid mid
  		grid:cell_collision(candidates,v,index)

  		-- 5: mid right
  		index2 = index + 1
  		grid:cell_collision(candidates,v,index2)

  		-- 6: bottom left
  		local index0 = index + grid.cw - 1 
  		grid:cell_collision(candidates,v,index0)

  		-- 7: bottom middle
  		local index1 = index0 + 1
  		grid:cell_collision(candidates,v,index1)

  		-- 8: bottom right
  		local index2 = index1 + 1
  		grid:cell_collision(candidates,v,index2)

      
  		for _,c in pairs(candidates) do
  			local res = self:intersect(v,c)

  			if (res ~= nil) then
  				local nx = res.x
  				local ny = res.y
  				self:apply_force(c, nx*amp/2,ny*amp/2)
  				local res2 = vec(0,0)-res
  				local nx = res2.x
  				local ny = res2.y
  				self:apply_force(v, nx*amp/2,ny*amp/2)			
  			end
  		end
  	end
  	]]
    
  	  	

  	
  	-- check bottom
  	
  	for k,v in pairs(self.liqs) do
  	  local inter = false
      if ((v.y + v.radius) > self.h) then
        local penetration = (v.y + v.radius) - self.h
        --[[local amount_pen = penetration / v.radius
        amount_pen = amount_pen * 15
        --penetration = penetration * 10
        self:apply_force(v, 0, -amount_pen)
        print(penetration / v.radius)]]
        v.vel[2] = -v.vel[2] * 0.9
        v.y = v.y - penetration
        inter = true
      end
      --end

      -- check left
      --for k,v in pairs(self.liqs) do
      if ((v.x + v.radius) > self.w) then
        local penetration = (v.x + v.radius) - self.w
        --[[local amount_pen = penetration / v.radius
        amount_pen = amount_pen * 15
        --penetration = penetration * 10
        self:apply_force(v, 0, -amount_pen)
        print(penetration / v.radius)]]
        v.vel[1] = -v.vel[1] * 0.9
        v.x = v.x - penetration
        inter = true
      end
      --end

      -- check right
      --for k,v in pairs(self.liqs) do
      if ((v.x - v.radius) < 0) then
        local penetration = (v.x - v.radius)
        --[[local amount_pen = penetration / v.radius
        amount_pen = amount_pen * 15
        --penetration = penetration * 10
        self:apply_force(v, 0, -amount_pen)
        print(penetration / v.radius)]]
        v.vel[1] = -v.vel[1] * 0.9
        v.x = v.x - penetration
        inter = true
      end


      if (inter) then
        v.vel[1] = v.vel[1] * 0.9
        v.vel[2] = v.vel[2] * 0.9
        v:undo_step()
      end
  	end
  	

    local max_vel = 16
    for k,v in pairs(self.liqs) do
      local l = vec(v.vel[1], v.vel[2])
      
      if math.abs(l:len()) > max_vel then
        l = l:norm()
        v.vel[1] = l.x * max_vel
        v.vel[2] = l.y * max_vel
      end
    end
		
    --[[self.old_x = self.x
    self.old_y = self.y
    
    -- f = ma, a = f / m, v = s/t, s = v * t, v = a * t = (f / m) * t
    self.vel[1] = self.vel[1] + (self.forces[1] / self.mass) * time
    self.vel[2] = self.vel[2] + (self.forces[2] / self.mass) * time
    
    local max_vel = 8
    local dir_vel = vec(self.vel[1], self.vel[2])
    local cur_vel = dir_vel:len()
    dir_vel = dir_vel:norm()
    if (cur_vel > max_vel) then
      cur_vel = max_vel
    end
    
    self.vel[1] = dir_vel[1] * cur_vel
    self.vel[2] = dir_vel[2] * cur_vel
    
    -- step pos
    self.x = self.x + self.vel[1] * time
    self.y = self.y + self.vel[2] * time
    
    -- reset forces
    self.forces = {0,0}
    ]]
  end
  
  return liqworld
end

function new_grid(w,h,cw,ch)
	local g = {}
	g.cells = {}
	g.cw = cw
	g.ch = ch
	g.step_x = w / cw
	g.step_y = h / ch
	g.max_index = ch * cw
	
	for i = 1,ch do
		for j = 1,cw do
			local x = j * g.step_x
			local y = j * g.step_y
			
			table.insert(g.cells,{x = x,y = y, items = { }})
		end
	end
	
	function g:cell_collision(candidates,a,i)
		if (i > 0) and (i <= self.ch*self.cw) then
			local items = self.cells[i].items
			
			for k,v in pairs(items) do
				if not (v == a) then
					table.insert(candidates,v)
				end
			end
		
		end
	end
	
	function g:remove(a)
		local index = a.cell_index
		
		if (index > 0) and (index <= self.ch*self.cw) then
		
			local items = self.cells[index].items
			
			for k,v in pairs(items) do
				if v == a then
					table.remove(items,k)
				end
			end
		
		end
	end
	
	function g:insert(a)
		local x = a.x
		local y = a.y
	
		local c_index = math.floor(x / g.step_x)
		local r_index = math.floor(y / g.step_y)
		
		local index = r_index * g.ch + c_index + 1
		
		a.cell_index = index
		table.insert(g.cells[index].items,a)
		--print(index)
	end
	
	return g
end


-----------------
-- lets do this

local world = create_liqworld(512,512)
--local a = create_liq(20,20, 10, 1, 1 )
for i=1,100 do
  local a = create_liq(math.random(20, 512-20), math.random(20, 50), 10, 1, 1 )
  a.vel[1] = math.random(-20, 20)
  world:add_liq(a)
end

--world:add_liq(a)

function update()
  -- step?
  world:step(1)
  
  local mx,my = inp.getmousepos()
  
  if inp.iskeydown(inp.ESC) then
		app.quit()
	end
	if inp.iskeydown(inp.TAB) then
		app.reboot()
	end
	
	if (inp.isbuttondown(inp.MOUSE_LEFT)) then
	  local a = create_liq(mx,my, 10, 1, 1)
    a.vel[1] = math.random(-5, 5)
    a.vel[2] = math.random(0, 50)
    world:add_liq(a)
  end
end




function draw()
  gfx.blending(gfx.SRC_ALPHA, gfx.ONE_MINUS_SRC_ALPHA)
  gfx.setclearcolor(1, 1, 1)
  gfx.clear()
  world:draw()
  -- lets draw the ballls
end


