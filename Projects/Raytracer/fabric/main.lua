require("fabric/guibase")
require("fabric/guistdwidgets")
require("fabric/specwindows")

----------------------------------------------
-- init gui
gui:init()
spawn_toolwindow()

----------------------------------------------
-- initial draw
gfx.redrawneeded()

function update()
  gui:update()
end

function draw(force)
  gui:draw(force)
  --gui:draw_custom_cursor(force)
end
