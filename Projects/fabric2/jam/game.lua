require("jam/physics")

-- game state
function new_game()
	local game = {}
	game.levels = {}
	game.current_level = nil
	game.gravity = vec(0.0,0.5)
	
	local freq = 1.0 / 60.0
	
	function game:update()
		-- physics simulation
		if(self.current_level) then
			-- apply forces and calculate position
			local grid = self.current_level.grid
			
			for k,v in pairs(self.current_level.actors) do
				v:add_force(self.gravity)
				
				-- get old grid cell index
				local p0 = v.body.body.c

				--v.body.velocity = v.body.velocity + self.gravity
				
				-- f = m*a, a = f / m
				local inv_m = 1 / v.body.mass
				local a = vec(v.body.force.x * inv_m,v.body.force.y * inv_m)
				
				v.body.velocity = v.body.velocity + vec(a.x*freq,a.y*freq)
				
				local p1 = v.body.body.c + vec(v.body.velocity.x * freq,v.body.velocity.y * freq)
				
				-- REMOVE THIS!!! use proper boundary checks later
				if(p1.x >= self.current_level.dimensions.w) then
					p1.x = self.current_level.dimensions.w
				end
				
				if(p1.y >= self.current_level.dimensions.h) then
					p1.y = self.current_level.dimensions.h
				end
				
				v.body.body.c = p1
			
				local x = v.body.body.c.x
				local y = v.body.body.c.y
				
				-- calculate cell index
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
			end
			
			-- check collisions
			for k,cell in pairs(grid) do
				
				
			
			end
			
			-- 
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
		
	return game
end


function new_level(name,num_actors,w,h)
	local level = {}
	level.actors = {}
	level.win_conditions = {}
	level.name = name
	level.grid = grid(w,h,8,8)
	level.dimensions = {w = w,h = h}
	
	for i=1,num_actors,1 do
		local x = 10.0 + i*15.0
		local y = 65.0
		
		local actor = generate_actor(x,y)
		
		table.insert(level.actors,actor)
		local c_index = math.floor(x / level.grid.step_x)
		local r_index = math.floor(y / level.grid.step_y)
		local index = c_index * level.grid.ch + c_index + 1
		
		actor.cell_index = index
		table.insert(level.grid.cells[index].items,actor)
	end
	
	return level
end	

function generate_actor(x,y)
	local hs = 5.0	-- half-size
	local r = math.sqrt(hs^2 + hs^2)
	
	local actor = {}
	actor.body = physics_body(x,y,5.0,1.0)
	actor.cell_index = nil
	
	function actor:draw(force)
		local p = self.body:position()
		gfx.bindtexture(0)
		gfx.setcolor(1.0,0.0,0.0)
		gfx.drawcentered(p.x, p.y, hs*2.0,hs*2.0)	
	end
	
	function actor:add_force(f)
		self.body.force = self.body.force + f
	end
	
	function actor:clear_forces()
		self.body:clear()
	end
	
	return actor
end

function grid(w,h,cw,ch)
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
	
	return g
end
