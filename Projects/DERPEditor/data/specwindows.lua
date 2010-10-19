function spawn_error_dialog(msg)
  local error_dialog = gui:create_window("errordialog"..msg[1], app.width / 2 - 250,app.height / 2 - 75, 500,120, true, "Error")
  local msg_panel = gui:create_centeredmultiline_label(0,10,500,100,msg)
  error_dialog.panel:addwidget(msg_panel)
  
  -- ok button
  local button = gui:create_labelbutton(210,60,90,30,"Ok", function(self) self.parent.parent:destroy() end)
  error_dialog.panel:addwidget(button)

  gui.windows:add(error_dialog)
end


function spawn_aboutwindow()
  local short_info = { "^(1,0.4,0.4){D}^(0.4,1,0.4){E}^(0.4,0.4,1){R}^(1,0.4,0.4){P} Editor is a bla bla bla,"
                     , "yada yada yada."
                     , "You might need to take a dump in your bed."
                     }
  
  local about_window = gui:create_window("aboutdialog", app.width / 2 - 200,app.height / 2 - 125,400,250, false, "About")
  local shortinfo_panel = gui:create_centeredmultiline_label(0,0,400,60,short_info)
  local memory_panel = gui:create_labelpanel(0,60,400,20,"Memory usage: unknown")
  
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
  
  gui.windows:add(about_window)
end

function spawn_settingswindow()
  local settings_window = gui:create_window("settingsdialog", app.width / 2 - 200,app.height / 2 - 200,400,400, false, "Application Settings")
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
