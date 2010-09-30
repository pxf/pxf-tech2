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



--[[local test = net.createserver()
for k,v in pairs(debug.getmetatable(test.instance)) do
  print(k,v)
end
--test.instance = nil]]

----------------------------------------------
-- setup menus
local file_menu = {{"Reboot", {tooltip = "Reboots the application. (Reloads all scripts and textures.)", onclick = function () app.reboot() end}},
				   {"Save", {tooltip = "Save current workspace.", onclick = function () derp:save(derp.active_workspace.widget_type,derp.active_workspace) end}}, -- this should open dialog etc
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
local about_menu = {{"Test Save Dialog", {onclick = function () print("Save to: " .. tostring(app.savedialog("workspace.derp"))) end}},
                    {"Test Load Dialog", {onclick = function () print("Load from: " .. tostring(app.opendialog())) end}},
                    {"Memory usage", {onclick = function () print("Memory usage: " .. tostring(app.getmemusage()/1024/1024) .. "MB") end}},
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
----------------------------------------------
-- create top widget stack
--   ie. menubar, console, toolbar, tabbar
--[[
local topstack = gui:create_verticalstack(0,0,app.width,10)

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
]]

----------------------------------------------
local menubar = gui:create_menubar(0,0,app.width,{{"File",file_menu},{"Edit", edit_menu},{"Window", window_menu},{"About", about_menu}})

local menu_container = derp:create_menu(0,0,app.width,40)
menu_container:addwidget(menubar)
	
local left_padding_container = derp:create_block(0,40,20,app.height-60, "left padding")
left_padding_container.border.right = true

local center_main_toolbar = derp:create_toolbar(20,40,app.width-40,40)
local testinput = gui:create_textinput(0,8,200)

--center_main_toolbar:addwidget(testinput)

local workspace_tabs = derp:create_workspace_tabs(20,80,app.width-290,20)
workspace_tabs.widget_type = "workspace tabs"

local workspace_area = derp:create_workspace(0,0,app.width,app.height)
workspace_area.widget_type = "workspace area 01"

local inspector = derp:create_inspector(app.width-270,80,250,app.height-100)

local right_padding_container = derp:create_block(app.width-20,40,20,app.height-60)
right_padding_container.widget_type = "right padding"
right_padding_container.border.left = true

local statusbar_container = derp:create_statusbar(0,app.height-20,app.width,20)

derp.active_workspace = workspace_area

ws01 = workspace_tabs:addtab("workspace01",workspace_area)

local workspace_frames = derp:create_workspaceframe(20,100,app.width-290,app.height-121)

gui.widgets:addwidget(workspace_area)
gui.widgets:addwidget(menu_container)
gui.widgets:addwidget(left_padding_container)
gui.widgets:addwidget(right_padding_container)
gui.widgets:addwidget(statusbar_container)
gui.widgets:addwidget(center_main_toolbar)
gui.widgets:addwidget(workspace_tabs)
gui.widgets:addwidget(inspector)
gui.widgets:addwidget(workspace_frames)

--workspace_area:addcomponent(0,0,"aux")

--gui.draw_debug_rects = false
--gui.draw_hitbox_rects = true
--gui.themetex = gfx.loadtexture("data/guitheme_brown.png")


--workspace_area.component_data = derp:load("test")


--derp:update()

----------------------------------------------
-- initial draw
gfx.redrawneeded()

function update()
  gui:update()
end

function draw(force)
  gui:draw(force)
end
