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
                                                 gridcount = 2,
                                                 clienthost = "dorsin.csbnet.se",
                                                 clientport = "50002",
                                                 localhost = "127.0.0.1",
                                                 localport = "4632"
                                                })
settings:load()
settings:save()

toolwindow, toolscroller = spawn_toolwindow()

----------------------------------------------
-- initial draw
gfx.redrawneeded()

last_tab = false

function update()
  if (inp.iskeydown(inp.TAB)) then
    if (not last_tab) then
      toolwindow.visible = not toolwindow.visible
      toolscroller.visible = not toolscroller.visible
    end
    
    last_tab = true
  else
    last_tab = false
  end
  gui:update()
end

function draw(force)
  local actual_count = 2^settings.data.gridcount
  for y=0,actual_count do
    for x=0,actual_count do
      panic.text(tostring(y*actual_count+x), x*(512/actual_count)+8, y*(512/actual_count)+8)
    end
  end
  gui:draw(true)
end
