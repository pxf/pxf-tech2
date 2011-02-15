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
                                                 toolpos = {0,0},
                                                 toolbarstate = "full",
                                                 clienthost = "dorsin.csbnet.se",
                                                 clientport = "50001",
                                                 localhost = "127.0.0.1",
                                                 localport = "4632"
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
