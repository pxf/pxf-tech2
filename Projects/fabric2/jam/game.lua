require("jam/physics")

-- game state
function new_game()
	local game = {}
	game.levels = {}
	game.current_level = nil
	
	function game:update()
		-- physics simulation
		if(self.current_level) then
			--self.current_level
		end
	end
	
	function game:draw(force)
		if self.current_level then
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


function new_level(name,num_actors)
	local level = {}
	level.actors = {}
	level.win_conditions = {}
	level.name = name
	level.dimensions = {x,y}
	
	for i=0,num_actors,1 do
		table.insert(level.actors,generate_actor(0.0,0.0))
	end
	
	return level
end	

function generate_actor(x,y)
	local hs = 5.0	-- half-size
	local r = math.sqrt(hs^2 + hs^2)
	
	local actor = {}
	actor.body = physics_body(x,y,5.0)
	
	function actor:draw(force)
		local p = self.body:position()
		gfx.setcolor(1.0,0.0,0.0)
		gfx.drawcentered(p.x, p.y, hs*2.0,hs*2.0)	
	end
	
	function actor:clear_forces()
		self.body:clear()
	end
	
	return actor
end