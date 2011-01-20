require("fabric/settingshandler")

----------------------------------------------
-- settings
settings = new_settings_handler("settings.ini", {rendersize = 512})
settings:load()
settings:save()

----------------------------------------------
-- initial draw
gfx.redrawneeded()

----------------------------------------------
-- logo animation
local a = 0
local logo = gfx.loadtexture(512, "fabric/logo.png")

gfx.setclearcolor(30/256, 0, 0)

function update()
  if inp.iskeydown(inp.ESC) then
    app.quit()
  end
  if inp.iskeydown(inp.F1) then
    print(app.framestats())
  end
  a = a + 0.004
end

function draw(force)
  gfx.bindtexture(logo)
  gfx.clear()
  for i=1,512 do
    gfx.drawcentered(i-1, app.height / 2 + math.sin(a-i*0.01) * 64 + math.cos((a-i*0.01)*0.6)*128, 1, 64, i-1+a*100, 0, 1, 64)
  end
end
