require("fabric/settingshandler")
require("jam/game")

----------------------------------------------
-- settings
settings = new_settings_handler("settings.ini", {rendersize = 512})
settings:load()
settings:save()

local game = new_game()

local level0 = new_level("LVL 1",1,512,512)
game:add_level(level0)
--game:add_level("LVL 1",32)

local last_keypress = nil

function update()
	if inp.iskeydown(inp.ESC) then
		app.quit()
	end
	if inp.iskeydown(inp.F1) then
		print(app.framestats())
	end
	
	if not (inp.iskeydown("R")) then
		if last_keypress == "R" then
			print("rebooting..")
			app.reboot()
			last_keypress = nil
		end
	else
		last_keypress = "R"
	end
	
	game:update()
end

function draw(force)
	gfx.clear()
	game:draw(force)
end
