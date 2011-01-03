require("fabric/guibase")
require("fabric/settingshandler")

----------------------------------------------
-- settings
settings = new_settings_handler("settings.ini", {rendersize = 512,
                                                 scale = 1,
                                                 toolpos = {0,0},
                                                 toolbarstate = "full"
                                                })
settings:load()
settings:save()

----------------------------------------------
-- initial draw
gfx.redrawneeded()

function update()
  
end

function draw(force)
  
end
