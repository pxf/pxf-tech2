require("jam/physics")

local STATE_RUNNING = 0
local STATE_PAUSED = 1

-- game state
function new_game()
	local game = {}
	game.levels = {}
	game.current_level = nil
	game.gravity = vec(0.0,20)
	game.state = STATE_RUNNING
	
	local freq = 1.0 / 60.0
	
	function game:update()
		-- physics simulation
		if(self.current_level and self.state == STATE_RUNNING) then
			-- apply forces and calculate position
			local grid = self.current_level.grid
			
			for k,v in pairs(self.current_level.actors) do
				if not v.static then
					v:add_force(self.gravity)
					
					-- get old grid cell index
					local p0 = v.body.body.c

					--v.body.velocity = v.body.velocity + self.gravity
					
					-- f = m*a, a = f / m
					local inv_m = 1 / v.body.mass
					local a = vec(v.body.force.x * inv_m,v.body.force.y * inv_m)
					
					v.body.velocity = v.body.velocity + vec(a.x*freq,a.y*freq)
					--print("id: " .. v.id .. " velocity: " .. v.body.velocity.x .. "," .. v.body.velocity.y)
					
					local p1 = v.body.body.c + v.body.velocity * freq
					
					if not v.rest then
						v:calc_position(freq)
					else
						v.body.velocity = vec(0,0)
					end
					
					
					--v.body.body.c = p1
					
					local x = v.body.body.c.x
					local y = v.body.body.c.y
					
					-- calculate cell index
					--[[
					local c_index = math.floor(x / self.current_level.grid.step_x)
					local r_index = math.floor(y / self.current_level.grid.step_y)
					local index = self.current_level.grid.cw * r_index + c_index + 1
					
					if not (index == v.cell_index) and index <= grid.max_index then
						local cells = self.current_level.grid.cells
						
						-- remove from old cell
						for k,a in pairs(cells[v.cell_index].items) do
							if v == a then
								cells[v.cell_index].items[k] = nil
							end
						end
						
						-- insert to new cell
						table.insert(cells[index].items,v)
						v.cell_index = index
					end
					]]
				end
			end
			
			local collision_hits = { }
			
			-- check collisions
			for k,v in pairs(self.current_level.actors) do
				--local cell = self.current_level.grid.cells[v.cell_index]
				
				-- check collision against every neighbouring cell
				-- gather items from all cells
				local candidates = {}
				
				--[[
				local index = v.cell_index
				
				-- 0: top left
				local index0 = index - grid.cw - 1 
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
				
				--]]
				
				--for k,a in pairs(candidates) do
				for _,a in pairs(self.current_level.actors) do
				
					local hit = nil
					if not (a == v) then
						hit = sphere_sphere_collision(a,v)
					end
					
					if hit then
						local c = {a = a,b = v, p = hit}
						
						-- find if pair already exist
						local found = false
						for _,cl in pairs(collision_hits) do
							--print("collision: " .. cl.a.id.. "," .. cl.b.id)
						
							if (cl.a == a and cl.b == v) or (cl.a == v and cl.b == a) then
								found = true
							end
						end
						
						if not found then
							table.insert(collision_hits,c)				
						end
					end
				end
				
				-- check walls collision 
				local p = v.body.body.c
				local r = v.body.body.r
				local p0 = p + vec(r,r)
				
				local impulse = v.body.velocity * 0.1
				local i_len = vec_len(impulse)
				
				--print(i_len)
				--[[
				if math.abs(vec_len(impulse)) < 0.1 then
					impulse = vec(0.0,0.0)
				end
				
				if p0.y >= self.current_level.dimensions.h then
					v.body.velocity = v.body.velocity - impulse * -1
					v.body.body.c = vec(p.x,512-5)  --p - vec(0,r)
				end
				--]]
				
				--v:calc_position(freq)
			end
			
			-- collision response?
			for k,v in pairs(collision_hits) do
				--("Lol")
				sphere_sphere_response(v.a,v.b)
				local a = v.a
				local b = v.b
				local p = v.p
				
				local p_dir = vec_norm(p)
				
				if not a.static then
					a.body.body.c = a.body.body.c - p_dir * a.body.body.r
				end
				
				if not b.static then
					b.body.body.c = b.body.body.c + p_dir * b.body.body.r
				end
				
				a:calc_position(freq)
				b:calc_position(freq)
			end
		end
	end
	
	function game:draw(force)
		if self.current_level then
			-- draw particles
			
			-- draw environment
			
			-- draw actors
			for k,v in pairs(self.current_level.actors) do
				v:draw(force)
				v:clear_forces()
			end
		end
	end
	
	function game:add_level(level)
		if (level) then
			table.insert(self.levels,level)
			
			self.current_level = level
		end
	end
	
	function game:pause()
		if self.state == STATE_RUNNING then
			self.state = STATE_PAUSED
		else
			self.state = STATE_RUNNING
		end
	end
		
	return game
end


function new_level(name,num_actors,w,h)
	local level = {}
	level.actors = {}
	level.win_conditions = {}
	level.name = name
	level.grid = new_grid(w,h,8,8)
	level.dimensions = {w = w,h = h}
	level.num_actors = 0
	
	function level:new_actor(x,y,static)
		local actor = generate_actor(x,y)
		actor.static = static
		
		table.insert(self.actors,actor)
		
		actor.id = self.num_actors
		self.grid:insert(actor)
		self.num_actors = self.num_actors + 1
		
		return actor
	end
	
	for i=1,num_actors,1 do
		local x = 10.0 + i*9
		local y = 65.0
		
		level:new_actor(x,y,false)
	end
	
	return level
end	

function generate_actor(x,y)
	local hs = 5.0	-- half-size
	local r = math.sqrt(hs^2 + hs^2)
	local actor = {}
	
	actor.body = physics_body(x,y,5.0,1.0)
	actor.cell_index = nil
	actor.static = false
	actor.rest = false
	
	function actor:draw(force)
		local p = self.body:position()
		gfx.bindtexture(0)
		gfx.setcolor(1.0,0.0,0.0)
		gfx.drawcentered(p.x, p.y, hs*2.0,hs*2.0)	
	end
	
	function actor:calc_position(freq)
		if not self.static then
			local len = vec_len(self.body.velocity) * freq
			local p1 = self.body.body.c + self.body.velocity * freq
			self.body.body.c = p1
			
			--print(self.body.velocity.y)
		end
	end
	
	function actor:add_force(f)
		self.body.force = self.body.force + f
	end
	
	function actor:clear_forces()
		self.body:clear()
	end
	
	return actor
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
			local x0 = j * g.step_x
			local y0 = j * g.step_y
			
			table.insert(g.cells,{x0 = x0,y0 = y0, items = { }})
		end
	end
	
	function g:cell_collision(candidates,a,i)
		if i <= 0 or i > self.max_index then
			return nil
		end
		
		local items = self.cells[i].items
		
		for k,v in pairs(items) do
			if not (v == a) then
				table.insert(candidates,v)
			end
		end
	end
	
	function g:insert(a)
		local x = a.body.body.c.x
		local y = a.body.body.c.y
	
		local c_index = math.floor(x / g.step_x)
		local r_index = math.floor(y / g.step_y)
		
		local index = r_index * g.ch + c_index + 1
		
		a.cell_index = index
		table.insert(g.cells[index].items,a)
	end
	
	return g
end
