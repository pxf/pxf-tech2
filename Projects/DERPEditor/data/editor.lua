require("data/guibase")
require("data/guistdwidgets")
require("data/specwindows")
require("data/DERPWidgets")

----------------------------------------------
-- "constants"
editor = {}
editor.name = "DERPEditor"
editor.version = "0.1"

----------------------------------------------
-- init GUI
gui:init()

local test = net.createserver()
for k,v in pairs(debug.getmetatable(test.instance)) do
  print(k,v)
end
test.instance = nil

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
local about_menu = {{"Memory usage", {onclick = function () print("Memory usage: " .. tostring(app.getmemusage()/1024/1024) .. "MB") end}},
                    {"About DERPEditor", {onclick = function () spawn_aboutwindow() end}}
                   }
				   
local window_menu = {{"Inspector",{toggle = false, tooltip = "Show/Hide inspector panel.", onclick = function() end}},
					 {"Toolbar",{toggle = true, tooltip = "Show/Hide Toolbar.",
						onclick = 
							function(self) 
								toolbar.visible = not self.toggle
								
								if toolbar.visible then
									toolbar:resize_abs(app.width,40)
								else
									toolbar:resize_abs(0,0)
								end
								
							end }},
					 {"Navigator",{toggle = false, tooltip = "Show/Hide navigator.", onclick = function() end}}
					}
                   
----------------------------------------------
-- create workspace

-- TODO: Create workspace.. :)

--local workspace = gui:create_workspace(

----------------------------------------------
-- create top widget stack
--   ie. menubar, console, toolbar, tabbar
local topstack = gui:create_verticalstack(0,0,app.width,10)
local menubar = gui:create_menubar(0,0,app.width,{{"File",file_menu},{"Edit", edit_menu},{"Window", window_menu},{"About", about_menu}})


console = gui:create_console(0,0,app.width,100,false)
toolbar = gui:create_horizontalpanel(0,0,app.width,40, app.width)
local testinput = gui:create_textinput(20,8,200)
toolbar:addwidget(testinput)

-- add topstack widgets
topstack:addwidget(menubar)
topstack:addwidget(toolbar)
topstack:addwidget(console)

----------------------------------------------
-- create status bar
local statusbar = gui:create_statusbar(0,app.height,app.width, editor.name .. " v" .. editor.version)
gui.statusbar = statusbar

----------------------------------------------
-- add widgets to root
--gui.widgets:addwidget(topstack)
--gui.widgets:addwidget(statusbar)

--[[
 _________________________
|WINDOW
| ___________________________
||MENU
||___________________________
||L|CENTER
|| |
||P|
||A|
||D|
||_|_________________________                           
||STATUSBAR
||___________________________
|____________________________

]]


----------------------------------------------
-- 0:
-- create attachment point
local window_container = derp:window_container()

	-- 1:
	local menu_container = derp:create_menu(0,0,app.width,40)
	menu_container:addwidget(menubar)

	-- 2:
	local center_container = gui:create_horizontalstack(0,0,app.width,app.height-60)
	center_container.widget_type = "center"

	-- 3: 
	local statusbar_container = derp:create_statusbar(0,0,app.width,20)

		-- 21: 
		local left_padding_container = derp:create_block(0,0,20,app.height-60, "left padding")
		left_padding_container.border.right = true

		-- 22
		local center_main_container = gui:create_verticalstack(20,0,app.width-40,app.height-60)
		center_main_container.widget_type = "center main container"
		
			--- 221
			local center_main_toolbar = derp:create_toolbar(0,0,app.width-40,40)
			local testinput = gui:create_textinput(20,8,200)
      center_main_toolbar:addwidget(testinput)

			--- 222 LOL no more names :(
			local center_main_main = derp:create_maincontainer(0,0,app.width-40,app.height-100)

				---- 2221
				local workspace = derp:create_workspacecontainer(0,0,app.width-290,app.height-100)

					----- 22211
					local workspace_tabs = derp:create_workspace_tabs(0,0,app.width-290,20)
					workspace_tabs.widget_type = "center: main: main: workspace: tabs"

					----- 22212 
					local workspace_area = derp:create_workspace(0,0,app.width-290,app.height-120)
					workspace_area.widget_type = "center: main: main: workspace: workspace area"
					
				---- 2222
				local inspector = derp:create_inspector(0,0,250,app.height-100)
				
		-- 23
		local right_padding_container = derp:create_block(app.width-20,0,20,app.height-60)
		right_padding_container.widget_type = "center container: right padding"
		right_padding_container.border.left = true

ws01 = workspace_tabs:addworkspace("workspace01")
ws01.active = true
ws02 = workspace_tabs:addworkspace("workspace02")

workspace:addwidget(workspace_tabs)
workspace:addwidget(workspace_area)

center_main_main:addwidget(workspace)
center_main_main:addwidget(inspector)

center_main_container:addwidget(center_main_toolbar)
center_main_container:addwidget(center_main_main)

center_container:addwidget(left_padding_container)
center_container:addwidget(center_main_container)
center_container:addwidget(right_padding_container)

window_container:addwidget(menu_container)
window_container:addwidget(center_container)
window_container:addwidget(statusbar_container)

gui.widgets:addwidget(window_container)
--gui.draw_debug_rects = true
--gui.themetex = gfx.loadtexture("data/guitheme_brown.png")

----------------------------------------------
-- initial draw
gfx.redrawneeded()

function update()
  gui:update()
end

function draw(force)
  --gfx.setalpha(1.0)
  gui:draw(force)
end
