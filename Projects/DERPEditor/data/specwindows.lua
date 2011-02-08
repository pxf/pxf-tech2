function spawn_error_dialog(msg)
  local error_dialog = gui:create_window("errordialog"..msg[1], app.width / 2 - 250,app.height / 2 - 75, 500,120, true, "Error")
  local msg_panel = gui:create_centeredmultiline_label(0,10,500,100,msg)
  error_dialog.panel:addwidget(msg_panel)
  
  -- ok button
  local button = gui:create_labelbutton(210,60,90,30,"Ok", function(self) self.parent.parent:destroy() end)
  error_dialog.panel:addwidget(button)

  gui.windows:add(error_dialog)
end

function spawn_log_window(msg)
  local window = gui:create_window("logwindow", app.width / 2 - 250,app.height / 2 - 75, 600,310, false, "Log")
  
  local log_panel = gui:create_multiline_label(0,0,590, 300,{})
  window.panel:addwidget(log_panel)
  
  function log_panel:update()
    self.lines = {}
    for i=#backlog.data,#backlog.data-16,-1 do
      if i < 1 then
        break
      end
      table.insert(self.lines, backlog.data[i])
    end
  end
  
  -- ok button
  --local button = gui:create_labelbutton(210,60,90,30,"Ok", function(self) self.parent.parent:destroy() end)
  --window.panel:addwidget(button)

  gui.windows:add(window)
end

function spawn_preview_window(component_data, w,h)
  local preview_dialog = gui:create_window("previewdialog" .. tostring(component_data.id), app.width / 2 - w/2,app.height / 2 - h/2, w+20,h+40, false, "Preview")
  
  
  preview_dialog.sdraw = preview_dialog.draw
  function preview_dialog:draw(force)
    self:sdraw(force)
    
    gfx.translate(self.drawbox.x+20,self.drawbox.y+40)
    self.prevdata.tex:draw(0,0,self.prevdata.w,0,self.prevdata.w,self.prevdata.h,0,self.prevdata.h)
    gfx.translate(-(self.drawbox.x+20),-(self.drawbox.y+40))
  end
  
  -- ok button
  --[[local button = gui:create_labelbutton(210,60,90,30,"Ok", function(self) self.parent.parent:destroy() end)
  preview_dialog.panel:addwidget(button)]]

  gui.windows:add(preview_dialog)
end


function spawn_aboutwindow()
  local width,height = 400,250
  local short_info = { "^(1,0.4,0.4){D}^(0.4,1,0.4){E}^(0.4,0.4,1){R}^(1,0.4,0.4){P} Editor is a bla bla bla,"
                     , "apa beba cepa."
                     , "TODO: Put some useful information here."
                     }
  
  local about_window = gui:create_window("aboutdialog", app.width / 2 - width/2,app.height / 2 - height/2,width,height, false, "About")
  local shortinfo_panel = gui:create_centeredmultiline_label(0,0,width,60,short_info)
  local memory_panel = gui:create_centeredlabelpanel(0,height-65,width-100,20,"Memory usage: unknown")
  
  about_window.panel:addwidget(shortinfo_panel)
  about_window.panel:addwidget(memory_panel)
  
  -- test memory stuff
  memory_panel.last_update = os.clock()
  function memory_panel:update()
    if (os.clock() - self.last_update > 0.1) then
      memory_panel.last_update = os.clock()
      memory_panel.label_text = "Memory usage: ^(0.4,0.4,1){" .. string.sub(tostring(app.getmemusage()/1024/1024), 1, 5) .. "}MB"
    end
  end
  
  -- ok button
  local button = gui:create_labelbutton(width-100,height-70,80,30,"Close", function(self) self.parent.parent:destroy() end)
  about_window.panel:addwidget(button)
  
  gui.windows:add(about_window)
end

function spawn_settingswindow()
  local settings_window = gui:create_window("settingsdialog", app.width / 2 - 200,app.height / 2 - 200,400,400, false, "Application Settings")
  local settings_stack = gui:create_verticalstack(30,50,400,20)
  settings_window:addwidget(settings_stack)
  
  local mouse_checkbox = gui:create_checkbox(0,50,200, "Custom mouse cursor", 
	function(self, value) 
		local toggle = not value
		gui:showmouse(toggle)
		derp.settings.mouse_state = toggle
		derp:store_settings()
	end)
	
  mouse_checkbox:toggle(not gui.mousevisible)
  settings_stack:addwidget(mouse_checkbox)
  

  --local shortinfo_panel = gui:create_centeredmultiline_label(0,0,400,60,short_info)
  --about_window.panel:addwidget(shortinfo_panel)
  --[[{"Settings", {menu = {{"GUI Settings", {menu = {{"Switch render mode", {tooltip = "Switch rendermodes; Fullscreen or stencil redraw.", onclick = function () if (app.rendermode == app.REDRAWMODE_FULL) then app.setrenderoption(app.REDRAWMODE_NORMAL) else app.setrenderoption(app.REDRAWMODE_FULL) end end}},
                                                   {"Show/hide redraw areas", {tooltip = "Show or hide redraw rectangles.", onclick = function () gui:toggle_show_redraw() end}}
                                                  }
                                          }
                        }}
                }
   }]]
   
  
  gui.windows:add(settings_window)
end
