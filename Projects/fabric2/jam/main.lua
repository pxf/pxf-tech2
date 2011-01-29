require("fabric/settingshandler")
require("jam/game")
require("jam/gui")

----------------------------------------------
-- settings
settings = new_settings_handler("settings.ini", {rendersize = 512, toolbarstate = "full"})
settings:load()
settings:save()

gamejam_init(settings)

local game = new_game()

local level0 = new_level("LVL 1",2,512,512)
level0:new_actor(25,400,true)

game:add_level(level0)

--game:add_level("LVL 1",32)

local last_keypress = nil


function toggle_button(b,fun)
	if not (inp.iskeydown(b)) then
		if last_keypress == b then
			fun()
			last_keypress = nil
		end
	else
		last_keypress = b
	end
end

function update()
	if inp.iskeydown(inp.ESC) then
		app.quit()
	end
	if inp.iskeydown(inp.F1) then
		print(app.framestats())
	end
	
	toggle_button("R",
		function () 
			print("rebooting..")
			app.reboot()
		end)
		
	toggle_button("P",
		function () 
			game:pause()
		end)
	
	gui:update()
	game:update()
end

function draw(force)
	gfx.clear()
	game:draw(force)
	
	gfx.loadidentity()
	gfx.setcolor(1, 1, 1)
	gui:draw(true)
end
