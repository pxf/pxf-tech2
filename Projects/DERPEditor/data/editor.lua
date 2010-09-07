require("data/guibase")
require("data/guistdwidgets")

--print("This is the editor!")

-- init GUI
gui:init()

-- add some cool widgets
local topstack = gui:create_verticalstack(0,0,app.width,60)
local console = gui:create_console(0,0,app.width,100,true)
console:addline("hey sup?")
console:addline("what are you? a HOMO?")
console:addline("you mad? 8)")
local rebootbutton = gui:create_simplebutton(12,12,100,30, function () app.reboot() end)
local quitbutton = gui:create_simplebutton(app.width - 78,12,60,30, function () app.quit() end)
local toolbar = gui:create_horisontalpanel(0,0,app.width,50, app.width)
--local movable_panel = gui:create_movablepanel(200, 200,100,100)
local movable_window = gui:create_movablewindow(200,200,100,100)

-- add buttons to toolbar
toolbar:addwidget(rebootbutton)
toolbar:addwidget(quitbutton)

-- add toolbar and console to topstack
topstack:addwidget(console)
topstack:addwidget(toolbar)

-- add topstack to root
gui.widgets:addwidget(topstack)
gui.widgets:addwidget(movable_window)

gfx.setclearcolor(113/256, 113/256, 113/256)

--test_texture = gfx.loadtexture("data/apperror.png")

function update()
  gfx.redrawneeded()
  gui:update()
end

function draw()
	gfx.scale(1.0)
  --gfx.bindtexture(test_texture)
  --gfx.drawcentered(300,300,256,128, 0, 0, 256, 128)
  --gfx.drawcentered(300,300,256,128)
  --gfx.setcolor(1, 0, 0)
  panic.text("Welcome to ^1D^2E^1R^2P^9 Editor!", 200, 200)
  --poooooooooop()
  
  gui:draw()
end
