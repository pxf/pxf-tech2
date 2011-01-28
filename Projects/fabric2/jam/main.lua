require("fabric/settingshandler")
require("jam/game")

----------------------------------------------
-- settings
settings = new_settings_handler("settings.ini", {rendersize = 512})
settings:load()
settings:save()

local game = new_game()

game:add_level("LVL 1",32)

function update()
	if inp.iskeydown(inp.ESC) then
		app.quit()
	end
	if inp.iskeydown(inp.F1) then
		print(app.framestats())
	end
	
	game:update()
end

function draw(force)
	gfx.clear()
	game:draw(force)
end
