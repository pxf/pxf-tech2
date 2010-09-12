require("data/guibase")
require("data/guistdwidgets")
require("data/specwindows")

-- init GUI
gui:init()

-- setup some menus
local file_menu = {{"Reboot", {tooltip = "Reboots the application. (Reloads all scripts and textures.)", onclick = function () app.reboot() end}},
                   {"Quit", {tooltip = "Quit the application.", shortcut = "Esc", onclick = function () app.quit() end}},
                  }
local edit_menu = {{"Copy", {tooltip = "Copy render block.", shortcut = "Ctrl-C", onclick = function () print("copy!!") end}},
                   {"Paste", {tooltip = "Paste render block.", shortcut = "Ctrl-V", onclick = function () print("paste!!") end}},
                   {"Settings", {menu = {{"GUI Settings", {menu = {{"Switch render mode", {tooltip = "Switch rendermodes; Fullscreen or stencil redraw.", onclick = function () if (app.rendermode == app.REDRAWMODE_FULL) then app.setrenderoption(app.REDRAWMODE_NORMAL) else app.setrenderoption(app.REDRAWMODE_FULL) end end}},
                                                                   {"Show/hide redraw areas", {tooltip = "Show or hide redraw rectangles.", onclick = function () gui:toggle_show_redraw() end}}
                                                                  }
                                                          }
                                        }}
                                }
                   }
                  }
local about_menu = {{"About DERPEditor", {onclick = function () spawn_aboutwindow() end}}
                   }

-- add some cool widgets
local topstack = gui:create_verticalstack(0,0,app.width)
local menustack = gui:create_menubar(0,0,app.width)
menustack:addwidget(gui:create_menubutton("File",file_menu))
menustack:addwidget(gui:create_menubutton("Edit",edit_menu))
menustack:addwidget(gui:create_menubutton("About",about_menu))
local console = gui:create_console(0,0,app.width,100,true)
console:addline("hey sup?")
console:addline("what are you? a HOMO?")
console:addline("you mad? 8)")
local rebootbutton = gui:create_simplebutton(6,6,130,30, "Reboot App", function () app.reboot() end)
local quitbutton = gui:create_simplebutton(app.width - 66,6,60,30, "Quit", function (self) app.quit() end)
local toolbar = gui:create_horisontalpanel(0,0,app.width,42, app.width)
function toolbar:mouserelease(mx,my,button)
  if (button == inp.MOUSE_RIGHT) then
    gui:spawn_menu(mx,my,file_menu)
  end
end
local statusbar = gui:create_statusbar(0,app.height,app.width,"DERPEditor")
gui.statusbar = statusbar

local movable_window = gui:create_movablewindow(200,200,140,100, "Supeeeeyh?")

-- add buttons to toolbar
toolbar:addwidget(rebootbutton)
toolbar:addwidget(quitbutton)

-- add menu, toolbar and console to topstack
topstack:addwidget(menustack)
topstack:addwidget(console)
topstack:addwidget(toolbar)

-- add topstack to root
gui.widgets:addwidget(topstack)
gui.widgets:addwidget(movable_window)
gui.widgets:addwidget(statusbar)

-- test sound
balls_id = snd.newsound("data/tick.ogg")
snd.playsound(balls_id)
snd.stopall()

-- initial draw
gfx.redrawneeded()

function update()
  gui:update()
end



function draw(force)
  gui:draw(force)
end
