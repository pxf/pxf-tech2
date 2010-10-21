require("data/vecmath")
require("data/splines")
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

net.addtag("pipeline")
net.addtag("datacache")
net.addtag("preview")
net.addtag("profiling")
net.addtag("log")

local fakenet = {connect = function () end, disconnect = function () end, send = function () end, recv = function () end}

local client = fakenet--net.createclient()
client:connect("localhost", 7005)
--client:recv()
client:send("pipeline", [[[{"blockName" : "PipelineTree",
"blockType" : "PipelineTree",
"blockData" : { "root" : "output1" }
},
{"blockName" : "auxinput1",
"blockType" : "AuxComp",
"blockData" : {"auxType" : "texture",
"filepath" : "data/derptest.png",
"minfilter" : "nearest"
},
"blockOutput" : [{"name" : "texture1",
"type" : "texture"}]
},
{"blockName" : "output1",
"blockType" : "Root",
"blockInput" : [{"block" : "auxinput1", "output" : "texture1"}],
"blockData" : {"host" : "localhost",
"port" : "4632",
"feedback" : true,
"realtime" : false,
"shaderVert" : "uniform sampler2D texture1;
void main(void)
{
gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
gl_TexCoord[0] = gl_MultiTexCoord0;
}",
"shaderFrag" : "uniform sampler2D texture1;
void main()
{
gl_FragColor = vec4(1.0);// - texture2D(texture1, gl_TexCoord[0].st);
}",
"width" : 512,
"height" : 512
}
}
]
]])
--[[
local retry = true
local aoe = nil

while (retry) do
  
  local daaaaata = client:recv()
  if (tostring(daaaaata.id) == "imgdata") then
    aoe = gfx.rawtexture(128, 512,512,4, tostring(daaaaata.data))
    retry = false
  else
    print("Got some strange package! :( '" .. tostring(daaaaata.id) .. "'")
  end
  
end]]

--client:disconnect()

----------------------------------------------

--local ws = nil

-- setup menus


file_menu = {{"Reboot", {tooltip = "Reboots the application. (Reloads all scripts and textures.)", onclick = 
									function () 	
										app.reboot() 
									end}},
				{"New", {tooltip = "Create a new workspace", onclick = 
									function() 
										derp:new_workspace(app.width*2,app.height*2)
									end}},
			   {"Open...", {tooltip = "Open previous workspace from file...", onclick = 
									function() 
										local filename = app.opendialog()
										
										if not (filename == nil) then 
											derp:open_workspace(filename,derp.active_workspace)
										else 
											print("Did not open!") 
										end 
									end}},
			   {"Open Recent...",{ menu = {  }
							}},
			   {"Save As...", {tooltip = "Save current workspace to file...", onclick = 
									function() 
										local filename = app.savedialog("workspace.derp")
										
										if not (filename == nil) then 
											derp:save(filename,derp.active_workspace) 
											derp:add_recent_file(filename)
										else 
											print("Did not save!") 
										end 
									end}},
               {"Quit", {tooltip = "Quit the application.", shortcut = "Esc", onclick = function () derp:store_settings() app.quit() end}},
                  }
local edit_menu = {{"Copy", {tooltip = "Copy render block.", shortcut = "Ctrl-C", onclick = function () print("copy!!") end}},
                   {"Paste", {tooltip = "Paste render block.", shortcut = "Ctrl-V", onclick = function () print("paste!!") end}},
				   {"Minimize all", {tooltip = "Minimize all components.", shortcut = "Alt-A", onclick = 
						function () 
							for k,v in pairs(derp.active_workspace.childwidgets) do
								v:set_state("minimized")
							end
						end}},
					{"Maximize all", {tooltip = "Maximize all components.", shortcut = "Alt-A", onclick = 
						function () 
							for k,v in pairs(derp.active_workspace.childwidgets) do
								v:set_state("maximized")
							end
						end}},
					{"Minimize all bodies", {tooltip = "Minimize all component bodies.", shortcut = "Alt-A", onclick = 
						function () 
							for k,v in pairs(derp.active_workspace.childwidgets) do
								v.body:set_state("minimized")
							end
						end}},
					{"Maximize all bodies", {tooltip = "Maximize all component bodies.", shortcut = "Alt-A", onclick = 
						function () 
							for k,v in pairs(derp.active_workspace.childwidgets) do
								v.body:set_state("maximized")
							end
						end}},
                   {"Settings", {tooltip = "Show settings dialog.", onclick = spawn_settingswindow}},
                   {"Quick Settings", {menu = {{"GUI Settings", {menu = {{"Switch render mode", {tooltip = "Switch rendermodes; Fullscreen or stencil redraw.", onclick = 
							function () 
								if (app.rendermode == app.REDRAWMODE_FULL) then 
									app.setrenderoption(app.REDRAWMODE_NORMAL) 
								else 
									app.setrenderoption(app.REDRAWMODE_FULL) 
								end 
							end}},
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
                   
local window_menu = {{"Log window", {onclick = function () spawn_log_window() end}},
           {"Inspector",{toggle = false, tooltip = "Show/Hide inspector panel.", onclick = function() end}},
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
inspector = derp:create_inspector(app.width-270,80,250,app.height-100)
workspace_tabs = derp:create_workspace_tabs(20,80,app.width-290,20)
workspace_frames = derp:create_workspaceframe(20,100,app.width-290,app.height-121)

local menubar = gui:create_menubar(0,0,app.width,{{"File",file_menu},{"Edit", edit_menu},{"Window", window_menu},{"About", about_menu}})
menubar.widget_type = "menubar"

local menu_container = derp:create_menu(0,0,app.width,40)
menu_container:addwidget(menubar)
	
local left_padding_container = derp:create_block(0,40,20,app.height-60, "left padding")
left_padding_container.border.right = true

local center_main_toolbar = derp:create_toolbar(20,40,app.width-40,40)
--center_main_toolbar:set_state("horizontal")
local testinput = gui:create_textinput(0,8,200)


workspace_tabs.widget_type = "workspace tabs"

local workspace_area = derp:create_workspace(0,0,app.width*2,app.height*2)
workspace_area.widget_type = "workspace area 01"

--inspector:addwidget(testinput)
--inspector:addwidget(derp:create_slider(0,0,100,0,1))

inspector:resize_abs(250,app.height-100)

local right_padding_container = derp:create_block(app.width-20,40,20,app.height-60)
right_padding_container.widget_type = "right padding"
right_padding_container.border.left = true

local statusbar_container = derp:create_statusbar(0,app.height-20,app.width,20)
--local statusbar = gui:create_statusbar(0,app.height,app.width, editor.name .. " v" .. editor.version)
gui.statusbar = statusbar_container

derp.active_workspace = workspace_area

ws01 = workspace_tabs:addtab("workspace01",workspace_area)

--local navigator = derp:create_navigator(app.width-421,app.height-172,150,150)

gui.widgets:addwidget(workspace_area)
gui.widgets:addwidget(menu_container)
gui.widgets:addwidget(left_padding_container)
gui.widgets:addwidget(right_padding_container)
gui.widgets:addwidget(statusbar_container)
gui.widgets:addwidget(center_main_toolbar)
gui.widgets:addwidget(workspace_tabs)
gui.widgets:addwidget(inspector)
gui.widgets:addwidget(workspace_frames)
--gui.widgets:addwidget(navigator)

derp.top_layer_id = #gui.widgets.childwidgets

derp:init()

--derp:open_workspace("recent_workspace.derp",ws)

----------------------------------------------
-- initial draw
gfx.redrawneeded()

function update()
  gui:update()
  
end

function draw(force)
  gui:draw(force)
  
  -- test line drawing:
  --[[local line = create_spline({{100,200},{200,100},{300,300},{400,200},{400,500}}, 30,2)
  line:update()
  local r,g,b = gfx.getcolor()
  local mx,my = inp.getmousepos()
  if (line:hit(mx,my,5)) then
    gfx.setcolor(1,0,0)
  else
    gfx.setcolor(1,1,1)
  end
  line:draw()
  gfx.setcolor(r,g,b)]]
  
  --aoe:draw(200,200,300,200,300,300,200,300)
  
  --[[if (#poopline > 3) then
    draw_spline(poopline, 100,2)
  end]]
end
