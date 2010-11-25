require("fabric/guibase")
require("fabric/guistdwidgets")
require("fabric/specwindows")
require("fabric/settingshandler")

----------------------------------------------
-- init gui
gui:init()
spawn_toolwindow()

----------------------------------------------
-- settings
settings = new_settings_handler("settings.ini", {rendersize = 512, scale = 1})
settings:load()
settings:save()

for k,v in pairs(settings.data) do
  print(k,v)
end

print(testcb())

----------------------------------------------
-- initial draw
gfx.redrawneeded()

function update()
  gui:update()
end

function draw(force)
  gui:draw(force)
end
