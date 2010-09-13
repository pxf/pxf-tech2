require("data/guibase")
require("data/guistdwidgets")
require("data/specwindows")

----------------------------------------------
-- "constants"
editor = {}
editor.name = "DERPEditor"
editor.version = "0.1"

----------------------------------------------
-- init GUI
gui:init()

----------------------------------------------
-- setup menus
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

local window_menu = {{"Inspector",{toggle = false, onclick = function() end}},
					 {"Toolbar",{toggle = true, onclick = function() end}},
					 {"Navigator",{toggle = false, onclick = function() end}}
					}
                   
----------------------------------------------
-- create workspace

-- TODO: Create workspace.. :)

----------------------------------------------
-- create top widget stack
--   ie. menubar, console, toolbar, tabbar
local topstack = gui:create_verticalstack(0,0,app.width)

local menubar = gui:create_menubar(0,0,app.width)
menubar:addwidget(gui:create_menubutton("File",file_menu))
menubar:addwidget(gui:create_menubutton("Edit",edit_menu))
menubar:addwidget(gui:create_menubutton("Window",window_menu))
menubar:addwidget(gui:create_menubutton("About",about_menu))

local console = gui:create_console(0,0,app.width,100,false)

local toolbar = gui:create_horisontalpanel(0,0,app.width,42, app.width)

-- add topstack widgets
topstack:addwidget(menubar)
topstack:addwidget(console)
topstack:addwidget(toolbar)

----------------------------------------------
-- create status bar
local statusbar = gui:create_statusbar(0,app.height,app.width, editor.name .. " v" .. editor.version)
gui.statusbar = statusbar


----------------------------------------------
-- add widgets to root
gui.widgets:addwidget(topstack)
gui.widgets:addwidget(statusbar)

----------------------------------------------
-- initial draw
gfx.redrawneeded()

function update()
  gui:update()
end

function draw(force)
  gui:draw(force)
end
