require("data/guibase")
require("data/guistdwidgets")

--print("This is the editor!")

-- init GUI
gui:init()

-- add some cool widgets
local topstack = gui:create_verticalstack(0,0,app.width)
local console = gui:create_console(0,0,app.width,100,true)
console:addline("hey sup?")
console:addline("what are you? a HOMO?")
console:addline("you mad? 8)")
local rebootbutton = gui:create_simplebutton(6,6,130,30, "Reboot App", function () app.reboot() end)
local quitbutton = gui:create_simplebutton(app.width - 66,6,60,30, "Quit", function (self) app.quit() end)
local testbutton = gui:create_simplebutton(app.width - 150,6,80,30, "Switch", function (self) app.setrenderoption(app.REDRAWMODE_NORMAL) end)--app.quit() end)
local toolbar = gui:create_horisontalpanel(0,0,app.width,42, app.width)
--local movable_panel = gui:create_movablepanel(200, 200,100,100)
local movable_window = gui:create_movablewindow(200,200,140,100)

-- add buttons to toolbar
toolbar:addwidget(rebootbutton)
toolbar:addwidget(quitbutton)
toolbar:addwidget(testbutton)

-- add toolbar and console to topstack
topstack:addwidget(console)
topstack:addwidget(toolbar)

-- add topstack to root
gui.widgets:addwidget(topstack)
gui.widgets:addwidget(movable_window)

--gfx.setclearcolor(113/256, 113/256, 113/256)

--test_texture = gfx.loadtexture("data/apperror.png")

balls_id = snd.newsound("data/tick.ogg")
snd.playsound(balls_id)
snd.stopall()

--print(balls_id)

--snd.playsound(balls_id,true)

gfx.redrawneeded()

--a = 0
--gfx.redrawneeded()

function update()
  --gfx.redrawneeded()
  --local mx, my = inp.getmousepos()
  --gfx.redrawneeded(mx,my,50,50)
  gui:update()
  --gfx.redrawneeded(200, 200, 200, 200)
  --gfx.redrawneeded()
  --a = a + 0.005
  --gfx.redrawneeded()
end



function draw(force)
  --[[gfx.bindtexture(test_texture)
  gfx.drawcentered(350,300,256,128, 0, 0, 256, 128)
  
  gfx.bindtexture(font)
  gfx.drawcentered(200,300,350,128, 0, 0, 64, 64)
  
  gfx.bindtexture(test_texture)
  gfx.drawcentered(300+math.cos(a)*100,300+math.sin(a)*100,256,128, 0, 0, 256, 128)]]

  --gfx.drawcentered(300,300,256,128)
  --gfx.setcolor(1, 0, 0)
  --panic.text("Welcome to ^1D^2E^1R^2P^9 Editor!", 200, 200)
  --poooooooooop()
  gui:draw(force, false)
end
