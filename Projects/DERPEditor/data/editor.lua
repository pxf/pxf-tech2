require("data/guibase")
require("data/guistdwidgets")

--print("This is the editor!")

-- init GUI
gui:init()

-- add some cool widgets
local rebootbutton = gui:create_simplebuttons(12,12,100,30, function () app.reboot() end)
local quitbutton = gui:create_simplebuttons(app.width - 78,12,60,30, function () app.quit() end)
local toolbar = gui:create_horisontalpanel(0,0,app.width,50)
toolbar:addwidget(rebootbutton)
toolbar:addwidget(quitbutton)
gui.widgets:addwidget(toolbar)

gfx.setclearcolor(113/256, 113/256, 113/256)

--test_texture = gfx.loadtexture("data/apperror.png")

function update()
  gfx.redrawneeded()
  gui:update()
end

function draw()
  --gfx.bindtexture(test_texture)
  --gfx.drawcentered(300,300,256,128, 0, 0, 256, 128)
  --gfx.drawcentered(300,300,256,128)
  --gfx.setcolor(1, 0, 0)
  panic.text("Welcome to ^1D^2E^1R^2P^9 Editor!", 200, 200)
  --poooooooooop()
  
  gui:draw()
end
