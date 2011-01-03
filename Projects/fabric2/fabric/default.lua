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

print("lol", "sup?")

----------------------------------------------
-- initial draw
gfx.redrawneeded()

function update()
  
end

function draw(force)
  gfx.bindtexture(runtimeerror_tex)
  gfx.drawcentered(app.width / 2, app.height / 2,512,256)
end
