require("fabric/settingshandler")
require("jam/game")
require("jam/gui")

----------------------------------------------
-- settings
settings = new_settings_handler("settings.ini", {rendersize = 512, toolbarstate = "full"})
settings:load()
settings:save()

gamejam_init(settings)

music = snd.newsound("data/sound/ggj11.ogg")

local game = new_game()

local level0 = new_level("LVL 1",0,512,512)

for i=1,10 do
	level0:new_actor(i*5,507,true)
end


for i = 1,8 do
	local x = math.random(35,45.5)
	local y = math.random(0,450.5)
	
	local s = math.random(0,1)
	
	if(s < 0.5) then
		s = true
	else
		s = false
	end
	
	local a = level0:new_actor(x,y,false)
end

level0:new_actor(40,500,true)

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
