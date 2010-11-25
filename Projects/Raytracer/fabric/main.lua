require("fabric/guibase")
require("fabric/guistdwidgets")
require("fabric/specwindows")
require("fabric/settingshandler")

----------------------------------------------
-- init gui
gui:init()

----------------------------------------------
-- settings
settings = new_settings_handler("settings.ini", {rendersize = 512,
                                                 scale = 1,
                                                 toolpos = {0,0}
                                                })
settings:load()
settings:save()

spawn_toolwindow()

----------------------------------------------
-- initial draw
gfx.redrawneeded()

function update()
  gui:update()
end

function draw(force)
  gui:draw(force)
end
