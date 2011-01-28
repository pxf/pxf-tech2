
-- game state
function new_game()
	local game = {}
	game.levels = {}
	game.current_level = nil
	
	function game:update()
		-- physics simulation
		if(self.current_level) then
			-- add gravity etc.
		end
	end
	
	function game:draw(force)
		
	end
	
	function game:add_level(name)
		
	end
		
	return game
end


function new_level(name,num_actors)
	local level = {}
	level.actors = {}
	level.win_conditions = {}
	level.name = name
	level.dimensions = {x,y}
	
	for i in range(num_actors) do
		table.insert(actors,generate_actor())
	end
	
	return level
end	

function generate_actor(x,y)
	local actor = {}
	
	return actor
end