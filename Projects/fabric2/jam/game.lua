
-- game state
function new_game()
	local game = {}
	game.levels = {}
	game.current_level = nil
	
	function game:update()
		-- physics simulation
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
	
	return level
end	

function generate_actor()
	local actor = {}
	actor.position = nil
	
	return actor
end